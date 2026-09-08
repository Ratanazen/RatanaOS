# Window System

Windows support open/close, focus/z-order, dragging, minimize/restore, and a
zoom toggle. Theme values drive rounded chrome, borders, shadows, alpha, and
traffic lights. `window_t.id` is now used for Dock, desktop, and menu actions,
so z-order rearrangement cannot retarget an application action.

General resize handles, fullscreen state, and a public application-facing
window manager remain future work. They are not claimed as implemented.
