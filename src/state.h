#ifndef STATE_H
#define STATE_H

#include <cmath>
#include <iostream>

const double g = 9.80665;

class state {
    public:
        double e[4];

        state() : e{0, 0, 0, 0} {}
        state(double e0, double e1, double e2, double e3) : e{e0, e1, e2, e3} {}

        state operator-() const { return state(-e[0], -e[1], -e[2], -e[3]); }
        double operator[](int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }

        inline state getDerivative(const state& u, double m1, double m2, double l1, double l2);
};

inline std::ostream& operator<<(std::ostream& out, const state& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2] << ' ' << v.e[3];
}

inline state operator+(const state& u, const state& v) 
{
    return state(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2], u.e[3] + v.e[3]);
}

inline state operator-(const state& u, const state& v) 
{
    return state(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2], u.e[3] - v.e[3]);
}

inline state operator*(double t, const state& v) {
    return state(t*v.e[0], t*v.e[1], t*v.e[2], t*v.e[3]);
}

inline state operator*(const state& v, double t) {
    return t * v;   
}

inline state operator/(const state& v, double t) {
    return (1/t) * v;
}

inline state getDerivative(const state& u, double m1, double m2, double l1, double l2)
{
    state derivative;
    derivative[0] = u[2];
    derivative[1] = u[3];

    // frequently used variables
    double theta1 = u[0];
    double theta2 = u[1];
    double omega1 = u[2];
    double omega2 = u[3]; 

    double dtheta = theta2 - theta1;
    double totalm = m1 + m2;

    double cosdtheta = cos(dtheta);
    double sindtheta = sin(dtheta);


    double alpha2 = (m2 * l2 * omega2 * omega2 * cosdtheta * sindtheta) + totalm * (g * cosdtheta * sin(theta1) + l1 * omega1 * omega1 * sindtheta - g * sin(theta2));
    alpha2 = alpha2 / (l2 * (totalm - m2 * cosdtheta * cosdtheta));

    double alpha1 = (-m2 * l2 * alpha2 * cosdtheta - m2 * l2 * omega2 * omega2 * sindtheta - totalm * g * sin(theta1)) / (totalm * l1);
    
    derivative[2] = alpha1;
    derivative[3] = alpha2;

    return derivative;
}

inline state rk4(const state &u, double m1, double m2, double l1, double l2, double dt)
{
    state f1 = getDerivative(u, m1, m2, l1, l2);
    state f2 = getDerivative(u + (dt/2) * f1, m1, m2, l1, l2);
    state f3 = getDerivative(u + (dt/2) * f2, m1, m2, l1, l2);
    state f4 = getDerivative(u + (dt) * f3, m1, m2, l1, l2);
    state next = u + (dt/6) * (f1 + 2*f2 + 2*f3 + f4);
    std::cout << (dt/6) * (f1 + 2*f2 + 2*f3 + f4) << '\n';
    std::cout << f1 << '\n';
    std::cout << f2 << '\n';
    std::cout << f3 << '\n';
    std::cout << f4 << '\n';
    std::cout << next << '\n';
    return next;
}

#endif