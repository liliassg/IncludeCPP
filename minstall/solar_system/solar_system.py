"""
REALISTIC SOLAR SYSTEM SIMULATION
==================================
All data from NASA JPL Horizons Database
Velocity Verlet integration for energy conservation
Powered by IncludeCPP for C++ performance

Controls:
  - Mouse scroll: Zoom in/out
  - Click + drag: Pan view
  - Keys 1-9: Focus on planet
  - Space: Pause/Resume
  - +/-: Change time scale
  - T: Toggle trajectories
  - I: Toggle inner/outer view
"""

import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.patches import Circle
import numpy as np
from includecpp import solar_system
import time

# Get constants
AU = solar_system.get_AU()
DAY = solar_system.get_DAY()
YEAR = solar_system.get_YEAR()

# Colors for each body (realistic-ish)
BODY_COLORS = {
    'Sun': '#FFD700',
    'Mercury': '#B5B5B5',
    'Venus': '#E6C35C',
    'Earth': '#4169E1',
    'Moon': '#C0C0C0',
    'Mars': '#CD5C5C',
    'Jupiter': '#DEB887',
    'Io': '#FFFF00',
    'Europa': '#F5DEB3',
    'Ganymede': '#A9A9A9',
    'Callisto': '#696969',
    'Saturn': '#F4D03F',
    'Titan': '#DAA520',
    'Uranus': '#87CEEB',
    'Neptune': '#1E90FF',
    'Triton': '#ADD8E6',
    'Pluto': '#D2B48C',
}

# Size multipliers for visibility
SIZE_MULT = {
    'Sun': 20,
    'Mercury': 4, 'Venus': 6, 'Earth': 6, 'Mars': 5,
    'Jupiter': 15, 'Saturn': 13, 'Uranus': 10, 'Neptune': 10,
    'Pluto': 3,
    'Moon': 2, 'Io': 2, 'Europa': 2, 'Ganymede': 2, 'Callisto': 2,
    'Titan': 2, 'Triton': 2,
}

print("="*70)
print("REALISTIC SOLAR SYSTEM SIMULATION")
print("Powered by IncludeCPP")
print("="*70)

# Initialize
ss = solar_system.SolarSystem()
ss.init_real_solar_system()
names = ss.get_names()

print(f"Bodies: {ss.get_body_count()}")
print(f"Initial energy: {ss.get_total_energy():.4e} J")
print()
print("Controls: Scroll=Zoom, 1-9=Focus, Space=Pause, +/-=Speed, T=Trails, I=Inner/Outer")
print()

# Simulation parameters
dt = 3600 * 6  # 6 hours per step
steps_per_frame = 10
time_scale = 1.0
paused = False
show_trails = True
view_mode = 'inner'  # 'inner' or 'outer'

# Create figure
fig, ax = plt.subplots(figsize=(12, 12), facecolor='black')
ax.set_facecolor('black')
ax.set_aspect('equal')
ax.axis('off')

# View limits
view_limits = {'inner': 3, 'outer': 50}
current_limit = view_limits[view_mode]
ax.set_xlim(-current_limit, current_limit)
ax.set_ylim(-current_limit, current_limit)

# Create scatter plot for bodies
pos_au = ss.get_positions_au()
x = [pos_au[i*3] for i in range(len(names))]
y = [pos_au[i*3+1] for i in range(len(names))]
colors = [BODY_COLORS.get(name, '#FFFFFF') for name in names]
sizes = [SIZE_MULT.get(name, 5) ** 2 for name in names]

scatter = ax.scatter(x, y, c=colors, s=sizes, zorder=10)

# Trail lines (one per body)
trail_lines = []
for i, name in enumerate(names):
    color = BODY_COLORS.get(name, '#FFFFFF')
    line, = ax.plot([], [], color=color, alpha=0.3, linewidth=0.5, zorder=5)
    trail_lines.append(line)

# Labels for major bodies
labels = []
for i, name in enumerate(names):
    if name in ['Sun', 'Mercury', 'Venus', 'Earth', 'Mars', 'Jupiter', 'Saturn', 'Uranus', 'Neptune', 'Pluto']:
        label = ax.text(0, 0, name, color='white', fontsize=7, ha='left', va='bottom', alpha=0.7)
        labels.append((i, label))

# Stats text
stats_text = ax.text(0.02, 0.98, '', transform=ax.transAxes,
                     color='white', fontsize=9, verticalalignment='top',
                     fontfamily='monospace',
                     bbox=dict(boxstyle='round', facecolor='black', alpha=0.7))

# Info text
info_text = ax.text(0.98, 0.02, '', transform=ax.transAxes,
                    color='white', fontsize=8, verticalalignment='bottom',
                    horizontalalignment='right', fontfamily='monospace')

frame_count = 0
frame_times = []

def on_scroll(event):
    global current_limit
    if event.button == 'up':
        current_limit *= 0.8
    else:
        current_limit *= 1.25
    current_limit = max(1, min(100, current_limit))
    ax.set_xlim(-current_limit, current_limit)
    ax.set_ylim(-current_limit, current_limit)

def on_key(event):
    global paused, time_scale, show_trails, view_mode, current_limit

    if event.key == ' ':
        paused = not paused
    elif event.key == '+' or event.key == '=':
        time_scale *= 2
    elif event.key == '-':
        time_scale = max(0.25, time_scale / 2)
    elif event.key == 't':
        show_trails = not show_trails
        for line in trail_lines:
            line.set_alpha(0.3 if show_trails else 0)
    elif event.key == 'i':
        view_mode = 'outer' if view_mode == 'inner' else 'inner'
        current_limit = view_limits[view_mode]
        ax.set_xlim(-current_limit, current_limit)
        ax.set_ylim(-current_limit, current_limit)
    elif event.key in '123456789':
        # Focus on planet (1=Mercury, 2=Venus, etc.)
        idx = int(event.key)
        if idx < len(names):
            pos = ss.get_positions_au()
            cx, cy = pos[idx*3], pos[idx*3+1]
            ax.set_xlim(cx - current_limit, cx + current_limit)
            ax.set_ylim(cy - current_limit, cy + current_limit)

fig.canvas.mpl_connect('scroll_event', on_scroll)
fig.canvas.mpl_connect('key_press_event', on_key)

def animate(frame):
    global frame_count, frame_times
    frame_count += 1

    if not paused:
        # Run physics
        start = time.perf_counter()
        actual_dt = dt * time_scale
        ss.simulate(actual_dt * steps_per_frame, actual_dt)
        physics_time = time.perf_counter() - start
        frame_times.append(physics_time)
        if len(frame_times) > 60:
            frame_times.pop(0)

    # Get positions
    pos_au = ss.get_positions_au()
    x = [pos_au[i*3] for i in range(len(names))]
    y = [pos_au[i*3+1] for i in range(len(names))]

    # Update scatter
    scatter.set_offsets(np.column_stack([x, y]))

    # Update labels
    for i, label in labels:
        label.set_position((x[i] + 0.1, y[i] + 0.1))

    # Update trails
    if show_trails:
        for i in range(len(names)):
            traj = ss.get_trajectory(i)
            if len(traj) > 6:
                tx = traj[0::3]
                ty = traj[1::3]
                trail_lines[i].set_data(tx, ty)

    # Stats
    years = ss.get_simulation_time_years()
    days = ss.get_simulation_time_days()
    avg_time = np.mean(frame_times) * 1000 if frame_times else 0

    earth_speed = ss.get_speed(3) / 1000
    earth_dist = ss.get_distance_from_sun(3) / AU

    stats_text.set_text(
        f"Time: {years:.2f} years ({days:.0f} days)\n"
        f"Time scale: {time_scale:.1f}x\n"
        f"Physics: {avg_time:.1f}ms/frame\n"
        f"Energy error: {ss.get_energy_error():.2e}\n"
        f"\n"
        f"Earth: {earth_dist:.3f} AU, {earth_speed:.1f} km/s\n"
        f"View: {view_mode} ({current_limit:.1f} AU)"
    )

    status = "PAUSED" if paused else "RUNNING"
    info_text.set_text(f"{status} | Space=Pause | +/-=Speed | T=Trails | I=View | Scroll=Zoom")

    return [scatter, stats_text, info_text] + trail_lines + [l for _, l in labels]

print("Starting animation...")
ani = animation.FuncAnimation(fig, animate, frames=None, interval=33, blit=False)
plt.tight_layout()
plt.show()

print(f"\nSimulation ended at {ss.get_simulation_time_years():.2f} years")
print(f"Final energy error: {ss.get_energy_error():.2e}")
