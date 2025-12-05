# sbokena game mechanics specification

1. _Tile_: immovable level elements, may contain at most 1 object.
2. _Object_: movable level elements.
3. _Tile_ types:
    1. _Floor_: freely enterable/exitable _Tile_.
    2. _Button_: hold-style button, activated when stepped on 
    and deactivated when stepped off. Can be activated both by 
    _Players_ and _Boxes_.
    3. _Door_: open-close door, triggered by one or more _Buttons_.
    A _Door_ can only be closed if there are no _Objects_ 
    underneath it, in which case the _Door_ doesn't close 
    until all _Objects_ exit it.
    4. _Portal_: bi-directional entrances. Each _Portal_ has a 
    designated direction in which it may be entered, and from 
    which it may be exited. All _Objects_ may traverse _Portals_.
    See the _Portal Traversal Rules_.
    5. _Uni-directional floor_: a _Floor_ which may only be 
    entered one-way, and exited the opposite way. If a 
    _Uni-directional box_ gets pushed onto a _Uni-directional floor_ 
    of a different direction, it becomes permanently stuck and 
    can no longer be pushed.
    6. _Goal_: a _Floor_ which the _Player_ aims to push _Boxes_ 
    onto. Once all _Goals_ have _Boxes_ on them, the level is 
    completed.
    7. _Wall_: any _Tile_ not of one of the above types is a _Wall_.
    A _Wall_ can not be entered. If a _Player_ tries to exit a 
    _Portal_ which faces a _Wall_ or a closed _Door_, they 
    should be rejected and stay in place.
4. _Object_ types:
    1. _Player_: a normal movable _Object_. State mutation operates 
    on an provided abstract input, which affects the primary 
    _Player_. A _Player_ activates a _Button_ when they step on 
    it. A _Player_ blocks a _Door_ from closing if they stand 
    inside it. A _Player_ may traverse a _Portal_. A _Player_ 
    may push _Boxes_, including _Uni-directional boxes_, 
    as detailed by the _Box Pushing Rules_.
    2. _Box_: a normal movable _Object_. A _Box_ may be pushed 
    by a _Player_, but may not itself push another _Box_ or 
    _Uni-directional box_. A _Box_ may traverse _Portals_.
    A _Box_ blocks _Doors_ from closing if it is "wedged" 
    under one. A _Box_ activates a _Goal_ when it is 
    placed on one, and deactivates it when it is pushed 
    off. A _Box_ activates _Buttons_ in a similar fashion.
    3. _Uni-directional box_: a _Box_ which can oly be pushed 
    in one direction. A _Uni-directional box_ may only 
    traverse a _Portal_ pair whose entrance and exit 
    directions are the same aas the _Box_'s direction.
5. Resolution Rules:
    1. _Box Pushing Rules_: When a _Player_ at position $P_{x,y}$ pushes a box 
    in direction $\text{D} \in \{ \text{Up, Down, Left, Right} \}$;
    let $\text{traverse}: P_{x,y} \rightarrow \text{D} \rightarrow P_{x,y}$ 
    be the position $P_{x,y}$ moved 1 unit over in direction $\text{D}$.
        - If ($\text{traverse } (\text{traverse } P_{x,y} \text{ D})\text{ D}$) 
    contains a _Wall_, a closed _Door_, or another _Box_, this _Box_ may not be pushed.
        - If ($\text{traverse } (\text{traverse } P_{x,y} \text{ D})\text{ D}$) enters a _Portal_, obtain the exit position $P'_{x,y}$ from the _Portal Traversal Rules_, then reapply the _Box Pushing Rules_ where the new position of the _Box_ is $P'_{x,y}$.
        - Otherwise, the _Box_ is pushed to position $\text{traverse } P_{x,y} \text{ D}$.
        - These rules also apply to _Uni-directional boxes_ with pushing direction $\text D$.
    2. _Portal Traversal Rules_: When a _Player_ at position $P_{x,y}$ moves in direction $\text{D} \in \{ \text{Up, Down, Left, Right} \}$ into a _Portal_ at position ($\text{traverse } P_{x,y} \text{ D}$) with entrance direction $\text D$, we consider them having entered the _Portal_.
        - When a _Player_ enters a _Portal_ P, consider the exit position $P'_{x,y}$:
            - If it contains a _Wall_ or a closed _Door_, the entrance is rejected, and the _Player_ stays in place.
            - If it contains a _Box_ or a _Uni-directional box_, and if the _Box_ can be pushed by the _Box Pushing Rules_ in the exit direction, push the _Box_ to $(\text{traverse } P'_{x,y})$, and move the _Player_ to $P'_{x,y}$. If box pushing fails, the _Player_ stay in place.
            - Otherwise, move the _Player_ to $P'_{x,y}$.
    3. _Moving Rules_: Every time a _Player_ successfully moves from a _Tile_ $T$ to a _Tile_ $T'$, the _Tile_ they just moved from is empty. Thus,
        1. Run the enter event on the next _Tile_ $T'$.
        2. Run the exit event on _Tile_ $T$.

        When a _Player_ successfully moves an _Object_, the order of events is:
        1. Run the enter event for _Object_ on $T''$.
        2. Run the exit event for _Object_ on $T'$.
        3. Run the enter event for _Player_ on $T'$.
        4. RUn the exit event for _Player_ on $T$.

        Should the _Player_ fails to move for multiple reasons, the failure
    order shall be as follows:
        1. Failure to exit a _Tile_:
            - Exiting a _DirFloor_ in the wrong direction.
        2. Failure to enter a _Tile_:
            - Entering a closed _Door_.
            - Entering a _Portal_ that can not be exited from. (See _Portal Traversal Rules_)
            - Entering a _Uni-directional floor_ in the wrong direction.
            - Entering a _Wall_.
