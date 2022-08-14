# Phase checker
The goal of this project is creating a tool for telling apart phases in a 3-phase power grid.

## Current state
It seems that with the mains frequency being quite unstable (accuracy is guaranteed over long periods of time, but there are short term deviations) it's practically impossible to keep a reference while moving from one outlet to another, if the distance is large. A potential solution would be two devices connected wirelessly.

## Goals:
### Hardware:
- Galvanically isolated mains input
- Battery powered so you can move between outlets
- Display

### Software:
- Some primitive PLL to eliminate drift
- Zeroing on the current phase
- Nice UI
