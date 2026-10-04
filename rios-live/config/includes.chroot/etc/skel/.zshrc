# ==============================================================================
# RiOS — User Zsh Shell Configuration (Host Style Parity)
# ==============================================================================

# PATH initialization
if [ -d "$HOME/.local/bin" ]; then
    export PATH="$HOME/.local/bin:$PATH"
fi
export PATH="/usr/local/bin:$PATH"

# Shell Options
setopt correct
setopt extendedglob
setopt nocaseglob
setopt nobeep
setopt appendhistory
setopt histignorealldups
setopt autocd

# History configuration
HISTFILE="$HOME/.zsh_history"
HISTSIZE=5000
SAVEHIST=10000

# Handy aliases
alias ll='ls -alF --color=auto'
alias la='ls -A --color=auto'
alias l='ls -CF --color=auto'
alias h='history'
alias f='fastfetch'

# Starship Prompt Integration
if command -v starship >/dev/null 2>&1; then
    eval "$(starship init zsh)"
else
    PROMPT='%F{blue}%~%f %F{cyan}❯%f '
fi
