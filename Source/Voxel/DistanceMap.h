#pragma once

#include <vector>
#include <functional>

#include <glm/glm.hpp>

#define INFTY 100000001
//https://tc18.org/DataSets/Code/SEDT/index.html
//Based on sedt
//sedt is a simple program to compute the squared Euclidean distance transform of a binary object (using the VOL format) in optimal time (i.e. in O(n) if n is the number of voxels). It is an implementation of the Saito and Toriwaki algorithm [SAITO_1994] with the optimization proposed by [HIRATA_1996] and [MEIJSTER_2000].
template<typename T>
class DistanceMap {
public:
  std::vector<T> distanceMap(const std::vector<bool>& data, const glm::u64vec3& resolution) {
    std::vector<T> sdt_x;
    sdt_x.resize(data.size());
    std::vector<T> sdt_xy;
    sdt_xy.resize(data.size());

    phaseSaitoX(data, sdt_x,resolution);
    phaseSaitoY(sdt_x, sdt_xy,resolution);
    phaseSaitoZ(sdt_xy, sdt_x, resolution); //We reuse sdt_x to store the final result!!

    return sdt_x;
  }

  std::vector<bool> map(const std::vector<T>& data, std::function<bool(T)> condition) {
    assert(data.size() % 8 == 0);
    std::vector<bool> result;
    result.resize(data.size());

#pragma omp parallel for
    for (long long i = 0; i < data.size(); i += 8) { //std::vector<bool> is special and partially not thread safe
      for (int64_t j = 0; j < 8; j++)
        result[i + j] = condition(data[i + j]);
    }
    return result;
  }

  std::vector<T> distanceMapXY(const std::vector<bool>& data, const glm::u64vec3& resolution) {
    std::vector<T> sdt_x;
    sdt_x.resize(data.size());
    std::vector<T> sdt_xy;
    sdt_xy.resize(data.size());

    phaseSaitoX(data, sdt_x, resolution);
    phaseSaitoY(sdt_x, sdt_xy, resolution);

    return sdt_xy;
  }

private:
  //operators.cc
  T sum(T a, T b)
  {
    if ((a == INFTY) || (b == INFTY))
      return INFTY;
    else
      return a + b;
  }
  T prod(T a, T b)
  {
    if ((a == INFTY) || (b == INFTY))
      return INFTY;
    else
      return a * b;
  }
  T opp(T a) {
    if (a == INFTY) {
      return INFTY;
    }
    else {
      return -a;
    }
  }
  T intdivint(T divid, T divis) {
    if (divis == 0)
      return  INFTY;
    if (divid == INFTY)
      return  INFTY;
    else
      return  divid / divis;
  }
  T F(int64_t x, int64_t i, T gi2)
  {
    return sum((x - i) * (x - i), gi2);
  }
  T Sep(int64_t i, int64_t u, T gi2, T gu2) {
    return intdivint(sum(sum((T)(u * u - i * i), gu2), opp(gi2)), 2 * (u - i));
  }
  int64_t getAddress(int64_t x, int64_t y, int64_t z, const glm::u64vec3& resolution) const {
    return z + resolution.z * y + resolution.z * resolution.y * x;
  }
  void phaseSaitoX(const std::vector<bool>& V, std::vector<T>& sdt_x, const glm::u64vec3& resolution)
  {
#pragma omp parallel for
    for (long long z = 0; z < resolution.z; z++) {
      for (int64_t y = 0; y < resolution.y; y++)
      {
        int64_t addressA = getAddress(0, y, z, resolution);
        if (V[addressA] == 0)
          sdt_x[addressA] = 0;
        else
          sdt_x[addressA] = INFTY;

        // Forward scan
        for (int64_t x = 1; x < resolution.x; x++) {
          int64_t addressB = getAddress(x, y, z, resolution);
          if (V[addressB] == 0)
            sdt_x[addressB] = 0;
          else {
            int64_t prev = getAddress(x - 1, y, z, resolution);
            T vv = sum(1, sdt_x[prev]);
            sdt_x[addressB] = vv;
          }
        }

        //Backward scan
        for (int64_t x = resolution.x - 2; x >= 0; x--) {
          int64_t backward = getAddress(x + 1, y, z, resolution);
          int64_t current = getAddress(x, y, z, resolution);
          if (sdt_x[backward] < sdt_x[current]) {
            T vv = sum(1, sdt_x[backward]);
            sdt_x[current] = vv;
          }
        }
      }
    }
  }
  void phaseSaitoY(const std::vector<T>& sdt_x, std::vector<T>& sdt_xy, const glm::u64vec3& resolution)
  {
    std::vector<int64_t> s;
    s.resize(resolution.y); //Center of the upper envelope parabolas
    std::vector<int64_t> t;
    t.resize(resolution.y); //Separating index between 2 upper envelope parabolas 


//#pragma omp parallel for
    for (int64_t z = 0; z < resolution.z; z++) {
      for (int64_t x = 0; x < resolution.x; x++)
      {
        int64_t q = 0;
        s[0] = 0;
        t[0] = 0;

        //Forward Scan
        for (int64_t u = 1; u < resolution.y; u++)
        {
          while ((q >= 0) &&
            (F(t[q], s[q], prod(sdt_x[getAddress(x, s[q], z, resolution)], sdt_x[getAddress(x, s[q], z, resolution)])) >
            F(t[q], u, prod(sdt_x[getAddress(x, u, z, resolution)], sdt_x[getAddress(x, u, z, resolution)])))
            )
            q--;

          if (q < 0)
          {
            q = 0;
            s[0] = u;
          }
          else
          {
            int64_t w = 1 + Sep(s[q],
              u,
              prod(sdt_x[getAddress(x, s[q], z, resolution)], sdt_x[getAddress(x, s[q], z, resolution)]),
              prod(sdt_x[getAddress(x, u, z, resolution)], sdt_x[getAddress(x, u, z, resolution)]));

            if (w < resolution.y)
            {
              q++;
              s[q] = u;
              t[q] = w;
            }
          }
        }

        //Backward Scan
        for (int64_t u = resolution.y - 1; u >= 0; --u)
        {
          sdt_xy[getAddress(x, u, z, resolution)] = F(u, s[q], prod(sdt_x[getAddress(x, s[q], z, resolution)], sdt_x[getAddress(x, s[q], z, resolution)]));
          if (u == t[q])
            q--;
        }
      }
    }
  }
  void phaseSaitoZ(const std::vector<T>& sdt_xy, std::vector<T>& sdt_xyz, const glm::u64vec3& resolution)
  {
    std::vector<int64_t> s;
    s.resize(resolution.z); //Center of the upper envelope parabolas
    std::vector<int64_t> t;
    t.resize(resolution.z); //Separating index between 2 upper envelope parabolas 

//#pragma omp parallel for
    for (int64_t y = 0; y < resolution.y; y++) {
      for (int64_t x = 0; x < resolution.x; x++)
      {
        int64_t q = 0;
        s[0] = 0;
        t[0] = 0;

        //Forward Scan
        for (int64_t u = 1; u < resolution.z; u++)
        {
          while ((q >= 0) &&
            (F(t[q], s[q], sdt_xy[getAddress(x, y, s[q], resolution)]) >
            F(t[q], u, sdt_xy[getAddress(x, y, u, resolution)]))
            )
            q--;

          if (q < 0)
          {
            q = 0;
            s[0] = u;
          }
          else
          {
            int64_t w = 1 + Sep(s[q],
              u,
              sdt_xy[getAddress(x, y, s[q], resolution)],
              sdt_xy[getAddress(x, y, u, resolution)]);

            if (w < resolution.z)
            {
              q++;
              s[q] = u;
              t[q] = w;
            }
          }
        }

        //Backward Scan
        for (int64_t u = resolution.z - 1; u >= 0; --u)
        {
          sdt_xyz[getAddress(x, y, u, resolution)] = F(u, s[q], sdt_xy[getAddress(x, y, s[q], resolution)]);
          if (u == t[q])
            q--;
        }
      }
    }
  }
};