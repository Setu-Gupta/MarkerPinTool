#include <marker.h>

volatile int x = 0;
int main()
{
        add_marker(START);
        add_marker(1);
        if(x > 0)
        {
                add_marker(2);
                add_marker(END);
                return x;
        }
        add_marker(END);
        return 0;
}
