
#include "component/circle.h"


int create_circle(
    CircleList& circles,
    const Circle::Args& args
){
    int index = circles.create();
    Circle& circle = circles[index];

    circle.radius = args.radius;
    circle.color = args.color;
    circle.edge_width = args.edge_width;

    return index;
}

void destroy_circle(
    CircleList& circles,
    int index)
{
    Circle& circle = circles[index];

    circle.radius = 0;

    circles.remove(index);
}
