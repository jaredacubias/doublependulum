#ifndef STATE_H
#define STATE_H

#include <cmath>
#include <iostream>

const double g = 9.80665;

class State 
{
    public:
        long double e[4];

        State() : e{0, 0, 0, 0} {}
        State(long double e0, long double e1, long double e2, long double e3) : e{e0, e1, e2, e3} {}

        State operator-() const { return State(-e[0], -e[1], -e[2], -e[3]); }
        long double operator[](int i) const { return e[i]; }
        long double& operator[](int i) { return e[i]; }

        float const x1(float root_x, float l1, int pixpermeter) { return root_x + (l1 * pixpermeter) * cos(e[0]); }
        float const x2(float root_x, float l1, float l2, int pixpermeter) { return this->x1(root_x, l1, pixpermeter) + (l2 * pixpermeter) * cos(e[1]); }
        float const y1(float root_y, float l1, int pixpermeter) { return root_y + (l1 * pixpermeter) * sin(e[0]); }
        float const y2(float root_y, float l1, float l2, int pixpermeter) { return this->y1(root_y, l1, pixpermeter) + (l2 * pixpermeter) * sin(e[1]); }

};

inline std::ostream& operator<<(std::ostream& out, const State& v) 
{
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2] << ' ' << v.e[3];
}

inline State operator+(const State& u, const State& v) 
{
    return State(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2], u.e[3] + v.e[3]);
}

inline State operator-(const State& u, const State& v) 
{
    return State(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2], u.e[3] - v.e[3]);
}

inline State operator*(double t, const State& v) {
    return State(t*v.e[0], t*v.e[1], t*v.e[2], t*v.e[3]);
}

inline State operator*(const State& v, double t) {
    return t * v;   
}

inline State operator/(const State& v, double t) {
    return (1/t) * v;
}

inline State getDerivative(const State& u, double m1, double m2, double l1, double l2)
{
    State derivative;
    derivative[0] = u[2];
    derivative[1] = u[3];

    // frequently used variables
    long double theta1 = u[0];
    long double theta2 = u[1];
    long double omega1 = u[2];
    long double omega2 = u[3]; 

    long double dtheta = theta2 - theta1;
    long double totalm = m1 + m2;

    long double cosdtheta = cos(dtheta);
    long double sindtheta = sin(dtheta);


    //long double alpha2 = 0.5 * m2 * l2 * omega2 * omega2 * sin(2 * dtheta) + totalm * (l1 * omega1 * omega1 * sindtheta - g * (cosdtheta * cos(theta1) + cos(theta2)));
    //alpha2 = alpha2 / (l2 * (m1 * cosdtheta * cosdtheta - totalm));

    long double alpha2 = cosdtheta * (m2*l2*omega2*omega2*sindtheta + totalm*g*cos(theta1)) + totalm * (l1*omega1*omega1*sindtheta - g*cos(theta2));
    alpha2 = alpha2 / (l2 * (m2*cosdtheta*cosdtheta - totalm));

    long double alpha1 = m2 *l2 * omega2 * omega2 * sindtheta - m1 * l2 * alpha2 * cosdtheta + totalm * g * cos(theta1);
    alpha1 = alpha1 / (totalm * l1);
    
    derivative[2] = alpha1;
    derivative[3] = alpha2;

    return derivative;
}

inline State rk4(const State &u, double m1, double m2, double l1, double l2, double dt)
{
    State f1 = getDerivative(u, m1, m2, l1, l2);
    State f2 = getDerivative(u + (dt/2) * f1, m1, m2, l1, l2);
    State f3 = getDerivative(u + (dt/2) * f2, m1, m2, l1, l2);
    State f4 = getDerivative(u + (dt) * f3, m1, m2, l1, l2);
    State next = u + (dt/6) * (f1 + 2*f2 + 2*f3 + f4);
    std::cout << (dt/6) * (f1 + 2*f2 + 2*f3 + f4) << '\n';
    std::cout << f1 << '\n';
    std::cout << f2 << '\n';
    std::cout << f3 << '\n';
    std::cout << f4 << '\n';
    std::cout << next << '\n';
    return next;
}

#endif