#pragma once

#include <vector>

#include <glm/glm.hpp>

#define INFTY 100000001
//https://tc18.org/DataSets/Code/SEDT/index.html
//Based on sedt
//sedt is a simple program to compute the squared Euclidean distance transform of a binary object (using the VOL format) in optimal time (i.e. in O(n) if n is the number of voxels). It is an implementation of the Saito and Toriwaki algorithm [SAITO_1994] with the optimization proposed by [HIRATA_1996] and [MEIJSTER_2000].
//template<typename T>
class DistanceMap {
public:
  //static std::vector<T> distanceMap(const std::vector<bool>& data, const glm::ivec3& resolution) {
  //  std::vector<T> result;

  //  return result;
  //}
private:
  //operators.cc
  long sum(long a, long b)
  {
    if ((a == INFTY) || (b == INFTY))
      return INFTY;
    else
      return a + b;
  }
  long prod(long a, long b)
  {
    if ((a == INFTY) || (b == INFTY))
      return INFTY;
    else
      return a * b;
  }
  long opp(long a) {
    if (a == INFTY) {
      return INFTY;
    }
    else {
      return -a;
    }
  }
  long intdivint(long divid, long divis) {
    if (divis == 0)
      return  INFTY;
    if (divid == INFTY)
      return  INFTY;
    else
      return  divid / divis;
  }
  long F(int x, int i, long gi2)
  {
    return sum((x - i) * (x - i), gi2);
  }
  long Sep(int i, int u, long gi2, long gu2) {
    return intdivint(sum(sum((long)(u * u - i * i), gu2), opp(gi2)), 2 * (u - i));
  }
  void phaseSaitoX(const std::vector<bool>& V, std::vector<long>& sdt_x, const glm::ivec3& resolution)
  {
    auto Vt = [&V, resolution](int x, int y, int z) -> bool {
      return V[z + resolution.z * y + resolution.z * resolution.y * x];
    };
    auto Lt = [&sdt_x, resolution](int x, int y, int z) -> long& {
      return sdt_x[z + resolution.z * y + resolution.z * resolution.y * x];
    };
    for (int z = 0; z < resolution.z; z++)
      for (int y = 0; y < resolution.y; y++)
      {
        if (Vt(0, y, z) == 0)
          Lt(0, y, z) = 0;
        else
          Lt(0, y, z) = INFTY;

        // Forward scan
        for (int x = 1; x < resolution.x; x++)
          if (Vt(x, y, z) == 0)
            Lt(x, y, z) = 0;
          else
            Lt(x, y, z) = sum(1, Lt(x - 1, y, z));;

        //Backward scan
        for (int x = resolution.x - 2; x >= 0; x--)
          if (Lt(x + 1, y, z) < Lt(x, y, z))
            Lt(x, y, z) = sum(1, Lt(x + 1, y, z));
      }
  }
  void phaseSaitoY(const std::vector<long>& sdt_x_, std::vector<long>& sdt_xy_, const glm::ivec3& resolution)
  {
    auto sdt_x = [&sdt_x_, resolution](int x, int y, int z) -> long {
      return sdt_x_[z + resolution.z * y + resolution.z * resolution.y * x];
    };
    auto sdt_xy = [&sdt_xy_, resolution](int x, int y, int z) -> long& {
      return sdt_xy_[z + resolution.z * y + resolution.z * resolution.y * x];
    };
    std::vector<int> s;
    s.resize(resolution.y); //Center of the upper envelope parabolas
    std::vector<int> t;
    t.resize(resolution.y); //Separating index between 2 upper envelope parabolas 
    int q;
    int w;

    for (int z = 0; z < resolution.z; z++)
      for (int x = 0; x < resolution.x; x++)
      {
        q = 0;
        s[0] = 0;
        t[0] = 0;

        //Forward Scan
        for (int u = 1; u < resolution.y; u++)
        {
          while ((q >= 0) &&
            (F(t[q], s[q], prod(sdt_x(x, s[q], z), sdt_x(x, s[q], z))) >
            F(t[q], u, prod(sdt_x(x, u, z), sdt_x(x, u, z))))
            )
            q--;

          if (q < 0)
          {
            q = 0;
            s[0] = u;
          }
          else
          {
            w = 1 + Sep(s[q],
              u,
              prod(sdt_x(x, s[q], z), sdt_x(x, s[q], z)),
              prod(sdt_x(x, u, z), sdt_x(x, u, z)));

            if (w < resolution.y)
            {
              q++;
              s[q] = u;
              t[q] = w;
            }
          }
        }

        //Backward Scan
        for (int u = resolution.y - 1; u >= 0; --u)
        {
          sdt_xy(x, u, z) = F(u, s[q], prod(sdt_x(x, s[q], z), sdt_x(x, s[q], z)));
          if (u == t[q])
            q--;
        }
      }
  }
  void phaseSaitoZ(const std::vector<long>& sdt_xy, std::vector<long>& sdt_xyz, const glm::ivec3& resolution)
  {
    auto xyz = [&sdt_xyz, resolution](int x, int y, int z) -> long& {
      return sdt_xyz[z + resolution.z * y + resolution.z * resolution.y * x];
    };
    auto xy = [&sdt_xy, resolution](int x, int y, int z) -> long{
      return sdt_xy[y + resolution.z * resolution.y * x];
    };
    std::vector<int> s;
    s.resize(resolution.z); //Center of the upper envelope parabolas
    std::vector<int> t;
    t.resize(resolution.z); //Separating index between 2 upper envelope parabolas 
    int q;
    int w;

    for (int y = 0; y < resolution.y; y++)
      for (int x = 0; x < resolution.x; x++)
      {
        q = 0;
        s[0] = 0;
        t[0] = 0;

        //Forward Scan
        for (int u = 1; u < resolution.z; u++)
        {
          while ((q >= 0) &&
            (F(t[q], s[q], xy(x, y, s[q])) >
            F(t[q], u, xy(x, y, u)))
            )
            q--;

          if (q < 0)
          {
            q = 0;
            s[0] = u;
          }
          else 
          {
            w = 1 + Sep(s[q],
              u,
              xy(x, y, s[q]),
              xy(x, y, u));

            if (w < resolution.z)
            {
              q++;
              s[q] = u;
              t[q] = w;
            }
          }
        }

        //Backward Scan
        for (int u = resolution.z - 1; u >= 0; --u)
        {
          xyz(x, y, u) = F(u, s[q], xy(x, y, s[q]));
          if (u == t[q])
            q--;
        }
      }
  }
};