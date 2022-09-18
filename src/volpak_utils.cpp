
#include "volpak.h"
#include "volpak_point.h"


double average(double low, double high){

	if (low < 0.0 || high < 0.0){
		throw std::invalid_argument("Negative height or diameter.");
	}

	return (low + high) / 2.0;
}


Point average(Point first, Point third){

    double radius = average(first.radius, third.radius);
    double hag = average(first.hag, third.hag);

    Point second(hag, radius);

    return second;

}


