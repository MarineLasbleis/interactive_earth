/************************ /
    StaggeredGrid.h
Define an abstract interface to a simple cartesian mesh,
with ways to iterate over the mesh cells, as well as query
for indexing and geometric information.  For use with
staggered cartesian grids in serial finite difference 
calculations
/ ***********************/

#include <iterator>
#include <iostream>
#include <cmath>

#ifndef STAGGERED_GRID_H
#define STAGGERED_GRID_H

struct Point
{
  double x;
  double y;
};

class StaggeredGrid
{
  public:

    class iterator;
    class reverse_iterator;

    class Cell 
    {
      public:

        Cell (unsigned int i, const StaggeredGrid &g): id(i), grid(g) {};
        Cell (Cell &c): id(c.id), grid(c.grid) {};
        Cell &operator=(const Cell &rhs) { id=rhs.id; return *this;}
 
        //get the x,y index of the cell
        int xindex() { return id % grid.nx; };
        int yindex() { return id / grid.nx; };
        
        //get grid indices of the neighbors
        int left() { return (id % grid.nx == 0 ? id+grid.nx-1 : id-1); };
        int right() { return ( (id+1)%grid.nx == 0 ? id-grid.nx+1 : id+1); };
        int up() { return (id + grid.nx >= grid.ncells ? id-grid.nx*(grid.ny-1) : id + grid.nx); };
        int down() { return ( id-grid.nx < 0 ? id+grid.nx*(grid.ny-1) : id-grid.nx) ; };
        int upleft() { return id + (id + grid.nx >= grid.ncells ? -grid.nx*(grid.ny-1) : grid.nx) + (id % grid.nx == 0 ? grid.nx-1 : -1); }
        int upright() { return id + (id + grid.nx >= grid.ncells ? -grid.nx*(grid.ny-1) : grid.nx) + ( (id+1)%grid.nx == 0 ? -grid.nx+1 : 1); }
        int downleft() { return id + (id-grid.nx < 0 ? grid.nx*(grid.ny-1) : -grid.nx) + ( id%grid.nx == 0 ? grid.nx-1 : -1); }
        int downright() { return id + (id+grid.nx <0 ? grid.nx*(grid.ny-1) : -grid.nx) + ( (id+1)%grid.nx == 0 ? -grid.nx+1 : 1); }
        int self() { return id; };
 
        //query for boundary information
        bool at_top_boundary() {return (id + grid.nx >= grid.ncells);};
        bool at_bottom_boundary() {return (id-grid.nx < 0);};
        bool at_left_boundary() {return (id%grid.nx == 0);};
        bool at_right_boundary() {return (id+1)%grid.nx == 0;};
        bool at_boundary() {return at_top_boundary() || at_bottom_boundary() || at_left_boundary() || at_right_boundary(); }

        //Get some location information
        Point center() { Point p; p.x = xindex()*grid.dx + grid.dx/2.0; p.y = yindex()*grid.dy + grid.dy/2.0;  return p;};
        Point corner() { Point p; p.x = xindex()*grid.dx; p.y = yindex()*grid.dy;  return p;};
        Point hface() { Point p; p.x = xindex()*grid.dx + grid.dx/2.0; p.y = yindex()*grid.dy;  return p;};
        Point vface() { Point p; p.x = xindex()*grid.dx; p.y = yindex()*grid.dy + grid.dy/2.0;  return p;};

        
      private:
        const StaggeredGrid& grid;
        int id;

      friend class StaggeredGrid::iterator;
      friend class StaggeredGrid::reverse_iterator;

    };

    class iterator : public std::iterator<std::input_iterator_tag, Cell>
    {
      private:
        int id;
        const StaggeredGrid& grid;
        Cell c;
      public:
        iterator(int i, const StaggeredGrid& g): id(i), grid(g), c(id,grid) {};
        iterator(const iterator &it) : id(it.id), grid(it.grid), c(id, grid) {};
        iterator& operator++() { ++id; ++c.id; return (*this);}
        iterator& operator++(int) { return operator++();}
        bool operator==(const iterator &rhs) {return id == rhs.id; };
        bool operator!=(const iterator &rhs) {return id != rhs.id; };
        Cell &operator*() {return c;}
        Cell* operator->() {return &c;}
    };
    class reverse_iterator : public std::iterator<std::input_iterator_tag, Cell>
    {
      private:
        int id;
        const StaggeredGrid& grid;
        Cell c;
      public:
        reverse_iterator(int i, const StaggeredGrid& g): id(i), grid(g), c(id,grid) {};
        reverse_iterator(const reverse_iterator &it) : id(it.id), grid(it.grid), c(id, grid) {};
        reverse_iterator& operator++() { --id; --c.id; return (*this);}
        reverse_iterator& operator++(int) { return operator++();}
        bool operator==(const reverse_iterator &rhs) {return id == rhs.id; };
        bool operator!=(const reverse_iterator &rhs) {return id != rhs.id; };
        Cell &operator*() {return c;}
        Cell* operator->() {return &c;}
    };
        
    //const members so we can query directly
    const double lx, ly; //Length in x,y directions
    const int nx, ny; //Number of cells in x,y directions
    const double dx, dy; //Number of cells in x,y directions
    const int ncells;

    StaggeredGrid(const double lenx, const double leny, const unsigned int numx, const unsigned int numy)
                  : lx(lenx), ly(leny), nx(numx), ny(numy), dx(lx/nx), dy(ly/ny), ncells(nx*ny) {}; 
    const iterator begin() { return iterator(0, *this); };
    const iterator end() {return iterator(ncells, *this);};
    const iterator rbegin() { return iterator(ncells-1, *this); };
    const iterator rend() {return iterator(-1, *this);};

    //Get handles to cell id and cell iterators at a point
    inline int cell_id( const Point &p) { int xindex = (p.x/dx); int yindex=(p.y/dy);
                                   return keep_in_domain(xindex, yindex); };
    inline int keep_in_domain( int xindex, int yindex) { return nx*(yindex < 0 ? 0 : (yindex >= ny ? ny-1: yindex))
                              + (xindex < 0 ? 0 : (xindex >= nx ? nx-1 : xindex)); };
    iterator cell_at_point( const Point &p) { return iterator(cell_id(p), *this); };

    iterator lower_left_corner_cell( const Point &p) { return cell_at_point(p); };
    iterator lower_left_hface_cell( const Point &p) { Point p2 = p; p2.x-= dx*0.5; return cell_at_point(p2); }; 
    iterator lower_left_vface_cell( const Point &p) { Point p2 = p; p2.y-= dy*0.5; return cell_at_point(p2); }; 
    iterator lower_left_center_cell( const Point &p) { Point p2 = p; p2.y-= dy*0.5; p2.x-= dx/2.0; return cell_at_point(p2); }; 
   
};


inline double lagrange_interp_2d( double x, double y, double ul, double u, double ur,
                                  double l, double c, double r, double dl, double d, double dr)
{
  double x2 = x*x; double y2 = y*y; double xy = x*y; 
  double x2y2 = x2*y2; double xy2 = x*y2; double x2y = x2*y;
  return   ul * (x2y2 + x2y - xy2 -xy)*0.25
         - u * (x2y2 + x2y - y2 - y)*0.5
         + ur * (x2y2 + x2y + xy2 + xy)*0.25
         - l * (x2y2 - x2 - xy2 + x )*0.5
         + c * (x2y2 - x2 - y2 + 1.0)
         - r * (x2y2 + -x2 + xy2 - x)*0.5
         + dl * (x2y2 - x2y - xy2 + xy)*0.25
         - d * (x2y2 - x2y - y2 + y )*0.5
         + dr * (x2y2 - x2y + xy2 -xy )*0.25;

/*  return   ul*(x)*(x-1.0)*(y)*(y+1.0)/4.0 
         - u*(x-1.0)*(x+1.0)*(y)*(y+1.0)/2.0 
         + ur*(x+1.0)*(x)*(y)*(y+1.0)/4.0
         - l*(x)*(x-1.0)*(y-1.0)*(y+1.0)/2.0
         + c*(x-1.0)*(x+1.0)*(y-1.0)*(y+1.0)
         - r*(x+1.0)*(x)*(y-1.0)*(y+1.0)/2.0
         + dl*(x)*(x-1.0)*(y)*(y-1.0)/4.0
         - d*(x-1.0)*(x+1.0)*(y)*(y-1.0)/2.0
         + dr*(x)*(x+1.0)*(y)*(y-1.0)/4.0;*/
}

inline double linear_interp_2d(double x, double y, double ul, double ur, double dl, double dr)
{
  return - ul * (x-1.0) * (y)
         + ur * (x) * (y)
         + dl * (x-1.0) * (y-1.0)
         - dr * (x) * (y-1.0);
}

#endif
