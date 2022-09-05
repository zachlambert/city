
# 1. ECS

# 2. Rendering

Only allow really simple rendering!
Essentially, everything is flat colours, with primitive shapes or polygons,
so no need for sprites/models.
Primitives:
- Circle: Radius, edge width (0 for filled), colour
- Rectangle: Width, height, angle, edge width (0 for filled), colour
- Line: Start, end, line width, colour
Polygons:
- Provide vertex and index list per usual, with a single flat colour for all vertices
- Different pipeline for static vs dynamic polygons

# 3. GUI

Write my own gui instead of using a library.
Make it immediate mode, and keep it simple for now.
Most important features
- Render text (in general, create a text renderer)
- Buttons which do something
- Layouts:
    - Single column, either growable or fixed height and scrollable
      width will expand as required, or if a fixed width is set, will pass
      this to children, which will wrap text as required.
    - Row, with things taking an automatic width, or enforcing alignment
