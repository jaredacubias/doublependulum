# Double Pendulum
this project simulates a double pendulum using rk4 and sfml

in order to compile using cmake:
```
cmake -B build
cmake --build build
```

to run with random initial conditions:
```/build/bin/main```

to run with fixed initial conditions (θ1, θ2, ω1, ω2):
```/build/bin/main θ1, θ2, ω1, ω2 ```