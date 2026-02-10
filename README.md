# Double Pendulum
this project simulates a double pendulum using rk4 and sfml. [this video](https://youtu.be/dBwwulr-M1s) demonstrates the simulation

the equations of motion used are the following:

$$\alpha_1=\frac{m_2\ell_2\omega_2^2\sin(\Delta\theta)+(m_1+m_2)g\cos(\theta_1)-m_2\ell_2\alpha_2\cos(\Delta\theta)}{\ell_1(m_1+m_2)}$$
$$\alpha_2=\frac{\cos(\Delta\theta)[m_2\ell_2\omega_2^2\sin(\Delta\theta)+(m_1+m_2)g\cos(\theta_1)]+(m_1+m_2)[\ell_1\omega_1^2\sin(\Delta\theta)-g\cos(\theta_2)]}{\ell_2[m_2\cos^2(\Delta\theta)-(m_1+m_2)]}$$

in order to compile using cmake:
```
cmake -B build
cmake --build build
```

to run with random initial conditions:
```/build/bin/main```

to run with fixed initial conditions (θ1, θ2, ω1, ω2):
```/build/bin/main θ1 θ2 ω1 ω2 ```
