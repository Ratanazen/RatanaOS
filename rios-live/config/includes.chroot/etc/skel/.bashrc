# ==============================================================================
# RiOS — User Bash Shell Configuration (Host Style Parity)
# ==============================================================================

# If not running interactively, don't do anything
case $- in
    *i*) ;;
      *) return;;
esac

# History control
HISTCONTROL=ignoreboth
shopt -s histappend
HISTSIZE=5000
HISTFILESIZE=10000

# Check window size after each command
shopt -s checkwinsize

# Colored GCC and tools
export GCC_COLORS='error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01'

# Color support for ls and grep
if [ -x /usr/bin/dircolors ]; then
    test -r ~/.dircolors && eval "$(dircolors -b ~/.dircolors)" || eval "$(dircolors -b)"
    alias ls='ls --color=auto'
    alias dir='dir --color=auto'
    alias vdir='vdir --color=auto'
    alias grep='grep --color=auto'
    alias fgrep='fgrep --color=auto'
    alias egrep='egrep --color=auto'
fi

# Handy aliases
alias ll='ls -alF --color=auto'
alias la='ls -A --color=auto'
alias l='ls -CF --color=auto'
alias h='history'
alias f='fastfetch'

# PATH initialization
export PATH="$HOME/.local/bin:/usr/local/bin:$PATH"

# Starship Prompt Integration
if command -v starship >/dev/null 2>&1; then
    eval "$(starship init bash)"
else
    # Fallback colored prompt if Starship is not installed
    PS1='\[\033[01;34m\]\w\[\033[00m\] \[\033[01;36m\]❯\[\033[00m\] '
fi
