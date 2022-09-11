

/* Defines */

#define NEWTON
#define STUMPHT 0.15



/* Tolerances */

static double HTTOL = 0.1;
static double RADTOL = 0.05/200;



/* Various functions */

double average(double low, double high);
Point average(Point, Point); 					// Surely this could be a template (combinations of std::shared_ptr<Point> and Point)
Point midpoint(Point base, Point top);

