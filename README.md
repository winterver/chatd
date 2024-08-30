<!--
# chatd
A simple chat server without ncurses, but can print newly received messages ABOVE your input line. This is done by using some complex linux console codes (man console_codes).

# bugs
You shall not post messages longer than the width of your terminal, or you'll see your input line overwritten (it's just a visual thing. the input buffer will not be overwritten).
-->

<!-- Rewritten by ChatGPT 4o mini -->
# chatd
Welcome to `chatd`, a straightforward chat server designed to display newly received messages above your input line. Unlike other chat servers, it does not use `ncurses`, but instead leverages advanced Linux console codes (see `man console_codes` for more details) to achieve this.

## Known Issues
- **Message Width Limitation**: Avoid sending messages longer than your terminal width. If a message exceeds the terminal's width, it may overwrite your input line. This issue is purely visual; your input buffer will not be affected.
