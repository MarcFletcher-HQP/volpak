#include <Rcpp.h>
#include <memory>
#include <vector>
#include <algorithm>


// [[Rcpp::plugins(cpp14)]]



class Point {

public:

  double getX(){return this->x;};
  double getY(){return this->y;};

  Point(){
    this->x = 0.0;
    this->y = 0.0;
  };

  Point(double x, double y){
    this->x = x;
    this->y = y;
  };


public:

  double x;
  double y;


};








// [[Rcpp::export]]
void const_ptr_test(Rcpp::NumericVector x, Rcpp::NumericVector y) {

  std::vector<std::unique_ptr<const Point>> pts;

  for(int i = 0; i < x.size(); i++){

    pts.push_back(std::move(std::make_unique<Point>(x[i], y[i])));

  }


  auto it = std::find_if(pts.begin(), pts.end(), [](const std::unique_ptr<Point> & elem){
    return elem->getX() < 0;
  });


  // attempt to modify const Point
  (*it)->x = 0.0;


  return ;
}




/*** R
const_ptr_test(rnorm(10), rnorm(10))
*/
