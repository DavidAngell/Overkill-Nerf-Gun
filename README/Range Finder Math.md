# Range Finder: Math

To start off, we are searching for an equation which takes in the range finder data, accelerometer data, and dart velocity and returns the angle at which the gun should be fired to hit the target.

Because geometry, if we know the measured range to a target and the angle at which it was measured,  we can determine the change in horizontal distance ($Δx$) and the change in vertical distance ($Δy$) to the target:

![range_finder_math_triangle.png](Range%20Finder%20Math/range_finder_math_triangle.png)

We also know the muzzle velocity ($v$) because we can measure it separately and get a good estimate of the approximate value of $v$. So, we need an equation which combines the change in horizontal distance ($Δx$), the change in vertical distance ($Δy$), the muzzle velocity ($v$), and, most importantly, the firing angle ($θ$). We can uses the kinematic equations for both the $x$ position and $y$ position given time, rearrange the first equation to solve for time, then substitute the result into the second equation, netting us an equation which contains all four of the variables that we either know or seek to know.

$$
\Delta x=vt\sin(\theta)
$$

$$
\Delta y=vt\cos(\theta)+ \frac{1}{2}gt^{2}
$$

$$
...
$$

$$
\Delta y = \Delta x\tan(\theta)+ \frac{{\Delta x}^{2}}{v^{2}{\cos}^{2}(\theta)}
$$

There is just one small problem: to my knowledge, at least, there is no way to solve for $θ$ in this equation. Feel free to give it a try! So, we need some algorithm that will allow us estimate $θ$. If I rearrange the equation, those who have taken a calculus course might see it.

$$
0 = \Delta x\tan(\theta) + \frac{{\Delta x}^{2}}{v^{2}{\cos}^{2}(\theta)} - \Delta y
$$

Newton’s method! A method for obtaining an approximation of the roots of a function. What we are really asking is when $f(θ) = 0$, which is just a root. Furthermore, the equation is pretty easily differentiable, so Newton’s method is perfect for this.

$$
θ_{n+1}=θ_{n}-\frac{f(θ_{n})}{f'(θ_{n})}
$$

$$
θ_{n+1}=θ_{n}-\frac{\Delta x\tan(θ_{n})+ {\sec}^{2}(θ_{n})\frac{{\Delta x}^{2}}{{v}^{2}} - \Delta y}{\Delta x\sec^{2}(θ_{n})+ 2\tan(θ_{n})\sec^{2}(θ_{n})\frac{{\Delta x}^{2}}{{v}^{2}} - \Delta y}
$$

Sure. It’s not the prettiest equation. But in code, there are actually a lot of things which only need to be calculated once per iteration: $tan(θ)$ and $sec^2(θ)$ only need to be calculated once per iteration and $\frac{Δx}{v^2}$ only needs to be calculated once per target. Overall, it could be worse.
