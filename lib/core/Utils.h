#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <config.h>

#include <algorithm>
// #include <iterator>
// #include <functional>

int clamp(int x, int upper, int lower)
{
    return min(upper, max(x, lower));
}

// int wrap(int pre) {
//   if (pre < 0) {
//     pre = pre + NUM_LEDS;
//   } else if (pre > NUM_LEDS) {
//     pre = pre - NUM_LEDS;
//   }
//   return pre;
// }

int map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

long map(double x, double in_min, double in_max, double out_min, double out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Quaternions helpers
float getAngularDistance(Quaternion &q1, Quaternion &q2)
{
    // https://www.mathworks.com/help/nav/ref/quaternion.dist.html
    float angularDistance = 2 * acos(q1.getProduct(q2.getConjugate()).w);
    // I added this, is it correct?
    if (angularDistance > M_PI)
        angularDistance = 2 * M_PI - angularDistance;

    return angularDistance;
}

void calculateEulerAngles(Quaternion q, int i, int j, int k, float &theta1, float &theta2, float &theta3)
{
    // From : https://www.ncbi.nlm.nih.gov/pmc/articles/PMC9648712/#pone.0276302.ref008
    bool not_proper;
    int epsilon;
    float a, b, c, d;

    if (i == k)
    {
        not_proper = false;
        k = 6 - i - j;
    }
    else
    {
        not_proper = true;
    }

    // Calculate epsilon
    epsilon = ((i - j) * (j - k) * (k - i)) / 2;

    double components[4] = {q.w, q.x, q.y, q.z};

    if (not_proper)
    {
        a = components[0] - components[j];
        b = components[i] + components[k] * epsilon;
        c = components[j] + components[0];
        d = components[k] * epsilon - components[i];
    }
    else
    {
        a = components[0];
        b = components[i];
        c = components[j];
        d = components[k] * epsilon;
    }

    theta2 = acos(2 * (a * a + b * b) / (a * a + b * b + c * c + d * d) - 1);
    double theta_plus = atan2(b, a);
    double theta_minus = atan2(d, c);

    if (theta2 == 0)
    {
        theta1 = 0;
        theta3 = 2 * theta_plus - theta1;
    }
    else if (theta2 == M_PI / 2)
    {
        theta1 = 0;
        theta3 = 2 * theta_minus + theta1;
    }
    else
    {
        theta1 = theta_plus - theta_minus;
        theta3 = theta_plus + theta_minus;
    }

    if (not_proper)
    {
        theta3 = epsilon * theta3;
        theta2 = theta2 - M_PI / 2;
    }

    // Wrap theta1 and theta3 as needed
}
#endif