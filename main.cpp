#include <iostream>
#include <cmath>

constexpr float X_MIN = -10;
constexpr float X_MAX = 10;
constexpr float Y_MIN = -10;
constexpr float Y_MAX = 10;

constexpr int IMAGE_X = 512;
constexpr int IMAGE_Y = 512;

float idxToReal(const float min, const float max, const int num_buckets, int idx)
{
    return min + (static_cast<float>(idx)/static_cast<float>(num_buckets))*(max - min);
}

float getRValue(int idx, int idy, float k)
{
    const float xdis = 0.0f;
    const float ydis = -5.0f;
    const float x = idxToReal(X_MIN, X_MAX, IMAGE_X, idx) - xdis;
    const float y = idxToReal(Y_MIN, Y_MAX, IMAGE_Y, idy) - ydis;
    return sqrt(0.5 * sin( k*sqrt(x*x + y*y) ) + 0.5);
}

float getGValue(int idx, int idy, float k)
{
    const float xdis = -5*(sqrt(3.0f)/2.0f);
    const float ydis = 5*(1.0f/2.0f);
    const float x = idxToReal(X_MIN, X_MAX, IMAGE_X, idx) - xdis;
    const float y = idxToReal(Y_MIN, Y_MAX, IMAGE_Y, idy) - ydis;
    return sqrt(0.5 * sin( k*sqrt(x*x + y*y) ) + 0.5);
}

float getBValue(int idx, int idy, float k)
{
    const float xdis = 5*(sqrt(3.0f)/2.0f);
    const float ydis = 5*(1.0f/2.0f);
    const float x = idxToReal(X_MIN, X_MAX, IMAGE_X, idx) - xdis;
    const float y = idxToReal(Y_MIN, Y_MAX, IMAGE_Y, idy) - ydis;
    return sqrt(0.5 * sin( k*sqrt(x*x + y*y) ) + 0.5);
}

int main()
{

    float r_array[IMAGE_X][IMAGE_Y];
    float b_array[IMAGE_X][IMAGE_Y];
    float g_array[IMAGE_X][IMAGE_Y];

    for (int idx = 0; idx < IMAGE_X; idx++)
    {
        for (int idy = 0; idy < IMAGE_Y; idy++)
        {
            r_array[idx][idy] = getRValue(idx, idy, 2*3.14159265/5);
            b_array[idx][idy] = getGValue(idx, idy, 2*3.14159265/5);
            g_array[idx][idy] = getBValue(idx, idy, 2*3.14159265/5);
            //printf("%f, ", r_array[idx][idy]);
        }
        printf("\n");
    }

    // Render

    std::cout << "P3\n" <<  IMAGE_X << ' ' << IMAGE_Y << "\n255\n";

    for (int idx = 0; idx < IMAGE_X; idx++) {

        std::cerr << "\rScanlines completed: " << idx << ' ' << std::flush;

        for (int idy = 0; idy < IMAGE_Y; idy++) {
            
            auto r = r_array[idy][idx];
            auto g = g_array[idy][idx];
            auto b = b_array[idy][idx];

            int ir = static_cast<int>(255.999 * r);
            int ig = static_cast<int>(255.999 * g);
            int ib = static_cast<int>(255.999 * b);

            std::cout << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }
    std::cerr << "\nDone.\n";
}
