- 50% Probability on **each**:
    - Intelligence
    - Telepathy
    - Tunneling Ability
    - Erratic Behavior

- Intelligence
    - Intelligent
        - Understands dungeon layout
        - Moves on shortest Path (path finding)
        - Remembers last known position of the last time they've seen the pc
    - Unintelligent (Blake)
        - Move in straight lines
- Telepathy
    - Telepathic
        - Always know where the pc is
        - Always move towards the pc
    - Non-telepathic (Blake)
        - Only know where the pc is if they can see it
        - Only moves toward visible PC
- Tunneling
    - I think already implemented??
- Erratic Behavior
    - Erractic monsters have a 50% chance of using their above characteristics
    - If they don't follow, they move to a random neighboring cell (tunneling still applies)


- Killing
    - The new arrival kills the original occupant


- Game Ends
    - PC is last alive
    - PC dies
    - Control + C

- Redraw the dungeon after each PC move, pause so that an observer can see the updates (use `usleep(3)`,
which sleeps for argument number of microseconds; something like 250000 is reasonable), and when the
game ends, print the win/lose status to the terminal before exiting.