#include  <iostream>
#include <math.h>
#include <vector>



int main(){

        const int size = 10000000;

#ifdef DOUBLE
        std::vector<double> array(size);
#else
        std::vector<float> array(size);
#endif

for (int i =0; i< size; ++i){

        double angle = (2*M_PI*i) /size;
        array[i] = std::sin(angle);

}
double sum = 0.0;
for (int i = 0; i < size; ++i){
        sum +=array[i];
}
std::cout << "result = " << sum << std::endl;



return 0;
}


