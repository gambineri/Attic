/*
 * Simulation for approx curve of Mu_p in the case of site percolation
 * in two dimension (finite percolation on a disk).
 */


#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>


#define MAX_DIST    10 // max disk radius
#define OCCUPIED    '+' // site is occupied
#define VACANT      '-' // site is vacant
#define SEEN        '*' // site has been visited
#define UNUSED      '.' // site has not been initialized (used)

#define CLOCKS_PER_MILLI  CLOCKS_PER_SEC/1000

char    m_Status[MAX_DIST*2 + 1][MAX_DIST*2 + 1];
char    m_Dist[MAX_DIST*2 + 1][MAX_DIST*2 + 1];
double  m_P = 0.0;
int     m_CurDist = 5;       // radius of disk (defaults to 5)

//-----------------------------------------------------------
void create_config(double p)
{
  memset(m_Status, UNUSED, sizeof(m_Status));

  for (int i=0; i<(MAX_DIST*2 + 1); i++)
    for (int j=0; j<(MAX_DIST*2 + 1); j++)
      if (m_Dist[i][j] <= m_CurDist)
        m_Status[i][j] = (((double)rand()/(RAND_MAX + 1)) < p) ? OCCUPIED : VACANT;
}

//-----------------------------------------------------------
void print_config()
{
  for (int i=0; i<(MAX_DIST*2 + 1); i++)
  {
    for (int j=0; j<(MAX_DIST*2 + 1); j++)
      if (m_Dist[i][j] <= m_CurDist)
        printf("%2c", m_Status[i][j]);
      else
        printf("%2c", ' ');

    printf("\n");
  }
  printf("\n");
}

//-----------------------------------------------------------
int config_level()
{
  int ret = 0;

  for (int i=0; i<(MAX_DIST*2 + 1); i++)
    for (int j=0; j<(MAX_DIST*2 + 1); j++)
      if (m_Dist[i][j] <= m_CurDist && m_Status[i][j] == OCCUPIED)
        ret++;
  
  return ret;
}

//-----------------------------------------------------------
void assign_distances()
{
  int i, j;

  for (i=0; i<MAX_DIST; i++)
  {
    for (j=0; j<MAX_DIST; j++)
      m_Dist[i][j] = MAX_DIST*2 - i - j;

    for (j=MAX_DIST; j<=MAX_DIST*2; j++)
      m_Dist[i][j] = j - i;
  }

  for (i=MAX_DIST; i<=MAX_DIST*2; i++)
  {
    for (j=0; j<MAX_DIST; j++)
      m_Dist[i][j] = i - j;

    for (j=MAX_DIST; j<=MAX_DIST*2; j++)
      m_Dist[i][j] = i - MAX_DIST*2 + j;
  }
}

//-----------------------------------------------------------
void print_distance_schema()
{
  for (int i=0; i<(MAX_DIST*2 + 1); i++)
  {
    for (int j=0; j<(MAX_DIST*2 + 1); j++)
      if (m_Dist[i][j] <= MAX_DIST)
        printf("%2d", m_Dist[i][j]);
      else
        printf("%2c", ' ');

    printf("\n");
  }
  printf("\n");
}

//-----------------------------------------------------------
char percolation_with_auto_build_path(int x, int y)
{
  if (m_Status[x][y] == UNUSED)
    m_Status[x][y] = (((double)rand()/(RAND_MAX + 1)) < m_P) ? OCCUPIED : VACANT;

  if (m_Status[x][y] == OCCUPIED)
  {
    if (m_Dist[x][y] == m_CurDist)
    {
//      m_Status[x][y] = SEEN; //only for 'visual' debug 
      return 1;
    }

    m_Status[x][y] = SEEN;

    if (percolation_with_auto_build_path(x + 1, y    )) return 1;
    if (percolation_with_auto_build_path(x - 1, y    )) return 1;
    if (percolation_with_auto_build_path(x    , y - 1)) return 1;
    if (percolation_with_auto_build_path(x    , y + 1)) return 1;
  }

  return 0;
}

//-----------------------------------------------------------
char percolation_on_existing_config(int x, int y)
{
  if (m_Status[x][y] == OCCUPIED)
  {
    if (m_Dist[x][y] == m_CurDist)
      return 1;

    m_Status[x][y] = SEEN;

    if (percolation_on_existing_config(x + 1, y    )) return 1;
    if (percolation_on_existing_config(x - 1, y    )) return 1;
    if (percolation_on_existing_config(x    , y - 1)) return 1;
    if (percolation_on_existing_config(x    , y + 1)) return 1;
  }

  return 0;
}

//-----------------------------------------------------------
// Approx curve for mu-p probability of percolation on Z2 disk
//-----------------------------------------------------------
void mup_on_disk(int from_dist, int to_dist, int x_points, int confs_per_point)
{
  long    beg;
  long    end;
  long    success  = 0;
  double  inc = 1/(double)x_points;
  FILE*   fp = NULL;
  char    fname[256];

  if (from_dist <= 0 || to_dist > MAX_DIST)
  {
    printf("Bad range parameters in mup_on_disk\n");
    return;
  }

  srand(time(NULL));
  assign_distances();

  beg = clock()*CLOCKS_PER_MILLI;

  for (m_CurDist = from_dist; m_CurDist<=to_dist; m_CurDist++)
  {
    sprintf(fname, "disk_z2_dist%03d.dat", m_CurDist);
    if ((fp = fopen(fname, "wt")) == NULL)
    {
      printf("Unable to open output file\n");
      return;
    }

    printf("Examinating disk %d\n", m_CurDist);
    fprintf(fp, "# Percolation on a finite disk in Z2 of radius %d\n", m_CurDist);

    m_P = 0.0;
    for (int points = 0; points<x_points; points++)  
    {
      m_P += inc;
      success = 0;
      for (int i=0; i<confs_per_point; i++)
      {
        memset(m_Status, UNUSED, sizeof(m_Status));
        success += percolation_with_auto_build_path(MAX_DIST, MAX_DIST);
      }

      fprintf(fp, "%f %f\n", m_P, success/(double)confs_per_point);
      printf("p = %f percolation frequency =  %f\n", m_P, success/(double)confs_per_point);
    }

    fprintf(fp, "\n\n");
    fclose(fp);
  }

  end = clock()*CLOCKS_PER_MILLI;
  printf("Done in %d seconds\n", (end - beg)/1000);
}

//-----------------------------------------------------------
// Approx curve for the number of pivotal sites
//-----------------------------------------------------------
void pivotal_sites(int from_dist, int to_dist, int x_points, int confs_per_point)
{
  int    beg;
  int    end;
  int    success  = 0;
  int    pivotals = 0;
  int    level = 0;
  double pivotals_fraction = 0.0;
  double inc = 1/(double)x_points;
  FILE*  fp = NULL;
  char   fname[256];
  char   config[MAX_DIST*2 + 1][MAX_DIST*2 + 1];

  if (from_dist <= 0 || to_dist > MAX_DIST)
  {
    printf("Bad range parameters in mup_on_disk\n");
    return;
  }

  srand(time(NULL));
  assign_distances();

  beg = clock()*CLOCKS_PER_MILLI;

  for (m_CurDist = from_dist; m_CurDist<=to_dist; m_CurDist++)
  {
    sprintf(fname, "disk_z2_pivot_dist%03d.dat", m_CurDist);
    if ((fp = fopen(fname, "wt")) == NULL)
    {
      printf("Unable to open output file\n");
      return;
    }

    printf("Examinating pivotal sites %d\n", m_CurDist);
    fprintf(fp, "# Pivotal sites on a finite disk in Z2 of radius %d\n", m_CurDist);

    m_P = 0.0;
    for (int points = 0; points<x_points; points++)  
    {
      m_P               += inc;
      success            = 0;
      pivotals_fraction  = 0.0;

      for (int conf=0; conf<confs_per_point; conf++)
      {
        create_config(m_P);
        memcpy(config, m_Status, sizeof(m_Status));
        
        //Is there percolation?
        if (percolation_on_existing_config(MAX_DIST, MAX_DIST) == 1)
        {
          pivotals = 0;
          memcpy(m_Status, config, sizeof(m_Status));
          level = config_level();

          //Count pivotal sites by checking on config one by one
          for (int i=0; i<(MAX_DIST*2 + 1); i++)
            for (int j=0; j<(MAX_DIST*2 + 1); j++)
              if (m_Dist[i][j] <= m_CurDist && m_Status[i][j] == OCCUPIED)
              {
                m_Status[i][j] = VACANT;
                if (percolation_on_existing_config(MAX_DIST, MAX_DIST) == 0)
                  pivotals++;
                memcpy(m_Status, config, sizeof(m_Status));
              }

          pivotals_fraction += pivotals / (double)level;
        }
      }

      if (pivotals_fraction > 0)
        fprintf(fp, "%f %f\n", m_P, pivotals_fraction/confs_per_point);

      printf("%f %f\n", m_P, pivotals_fraction/confs_per_point);
    }

    fprintf(fp, "\n\n");
    fclose(fp);
  }

  end = clock()*CLOCKS_PER_MILLI;
  printf("Done in %d seconds\n", (end - beg)/1000);
}

int main(int argc, char* argv[])
{
//  mup_on_disk(100, 100, 100, 300000);

  pivotal_sites(10, 10, 100, 10000);

	return 0;
}
