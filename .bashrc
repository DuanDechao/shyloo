# ~/.bashrc: executed by bash(1) for non-login shells.
# see /usr/share/doc/bash/examples/startup-files (in the package bash-doc)
# for examples

# If not running interactively, don't do anything
[ -z "$PS1" ] && return

# don't put duplicate lines in the history. See bash(1) for more options
# HISTCONTROL来消除命令历史中的连续重复条目
export HISTCONTROL=$HISTCONTROL${HISTCONTROL+,}ignoredups
# ... and ignore same sucessive entries.
export HISTCONTROL=ignoreboth

# for setting history length see HISTSIZE and HISTFILESIZE in bash(1)
HISTSIZE=2000
HISTFILESIZE=4000
export HISTTIMEFORMAT="%F %T:"

# append to the history file, don't overwrite it
shopt -s histappend

# for setting history length see HISTSIZE and HISTFILESIZE in bash(1)

# check the window size after each command and, if necessary,
# update the values of LINES and COLUMNS.
shopt -s checkwinsize

# make less more friendly for non-text input files, see lesspipe(1)
#[ -x /usr/bin/lesspipe ] && eval "$(SHELL=/bin/sh lesspipe)"


# set a fancy prompt (non-color, unless we know we "want" color)
case "$TERM" in
	xterm-color)color_prompt=yes;;
esac

# uncomment for a colored prompt, if the terminal has the capability; turned
# off by default to not distract the user: the focus in a terminal window
# should be on the output of commands, not on the prompt
#force_color_prompt=yes

if [ -n "$force_color_prompt" ]; then
    if [ -x /usr/bin/tput ] && tput setaf 1 >&/dev/null; then
	# We have color support; assume it's compliant with Ecma-48
	# (ISO/IEC-6429). (Lack of such support is extremely rare, and such
	# a case would tend to support setf rather than setaf.)
	color_prompt=yes
    else
	color_prompt=
    fi
fi

unset color_prompt force_color_prompt

# Alias definitions.
# You may want to put all your additions into a separate file like
# ~/.bash_aliases, instead of adding them here directly.
# See /usr/share/doc/bash-doc/examples in the bash-doc package.

if [ -f ~/.bash_aliases ]; then
    . ~/.bash_aliases
fi

# enable color support of ls and also add handy aliases
if [ -x /usr/bin/dircolors ]; then
	eval "`dircolors -b`"
    alias ls='ls -CF --color=auto --hide=cscope.* --hide=tags'
    alias grep='grep --color=auto'
    alias fgrep='fgrep --color=auto'
    alias egrep='egrep --color=auto'
    alias ll='ls -lh'
fi

if [ -f ~/.myvim/.bash_aliases ]; then
    . ~/.myvim/.bash_aliases
fi

# enable programmable completion features (you don't need to enable
# this, if it's already enabled in /etc/bash.bashrc and /etc/profile
# sources /etc/bash.bashrc).
if [ -f /etc/bash_completion ]; then
    . /etc/bash_completion
fi

#设置vi模式
#set -o vi

localip=`/sbin/ifconfig eth0 2>/dev/null | awk '$1=="inet"{print $2}' | awk -F: '{print $2}' `
export LOCAL_IP=$localip
export LOCAL_IP_4=`echo $localip | awk -F. '{print $4}' `
export LOCAL_IP_3=`echo $localip | awk -F. '{print $3}' `
export PS1='\[\e[01;35m\]==\[\e[01;31m\][$LOCAL_IP_4]\[\e[01;35m\]==\[\e[01;36m\][\[\e[01;37m\]\u\[\e[01;36m\]\[\e[00m\] \[\e[01;34m\]`pwd``B=$(git branch 2>/dev/null | sed -e "/^ /d" -e "s/* \(.*\)/\1/"); if [ "$B" != "" ]; then S="git"; elif [ -e .bzr ]; then S=bzr; elif [ -e .hg ]; then S="hg"; elif [ -e .svn ]; then S="svn"; else S=""; fi; if [ "$S" != "" ]; then if [ "$B" != "" ]; then M=$S:$B; else M=$S; fi; fi; [[ "$M" != "" ]] && echo -n -e "\[\e[32;40m\]($M)\[\e[01;32m\]\[\e[00m\]"`\[\e[01;34m\]\[\e[01;36m\]]\[\e[01;31m\]$ \[\e[00m\]'
#export PS1='\[\e[01;35m\]==\[\e[01;31m\][$LOCAL_IP_4]\[\e[01;35m\]==\[\e[01;36m\][\[\e[01;37m\]\u\[\e[01;36m\]\[\e[00m\] \[\e[01;34m\]`pwd`\[\e[01;34m\]\[\e[01;36m\]]\[\e[01;31m\]$ \[\e[00m\]'
export TERM=xterm-256color
#export PROMPT_COMMAND="echo -ne \"\033]0;==[$LOCAL_IP_4]==[`whoami`]\007\""
export PROMPT_COMMAND="echo -ne \"\033]0;==[$LOCAL_IP]==[`whoami`]\007\""
export EDITOR="vim"
export LANG=en_US.UTF-8
ulimit -c 40000000

if [ -x /usr/bin/dircolors ]; then
    test -r ~/.dircolors && eval "$(dircolors -b ~/.dircolors)" || eval "$(dircolors -b)"
fi

export LESS_TERMCAP_mb=$'\E[01;31m'
export LESS_TERMCAP_md=$'\E[01;31m'
export LESS_TERMCAP_me=$'\E[0m'
export LESS_TERMCAP_se=$'\E[0m'
export LESS_TERMCAP_so=$'\E[01;44;33m'
export LESS_TERMCAP_ue=$'\E[0m'
export LESS_TERMCAP_us=$'\E[01;32m'

#绑定bash快捷键 绑定的函数见man readline
#清屏
bind -m vi-insert '\c-l':clear-screen
#向前 向后搜索历史
bind -m vi-insert '\c-n':history-search-forward
bind -m vi-insert '\c-p':history-search-backward
#跳到末尾
bind -m vi-insert '\c-e':end-of-line
#跳到开头
bind -m vi-insert '\c-a':beginning-of-line
#向后一个字符
bind -m vi-insert '\c-b':backward-char
#向前一个字符
bind -m vi-insert '\c-f':forward-char
#向前del一个字符
bind -m vi-insert '\c-d':forward-delete-char
bind -m vi-insert '\c-x':exchange-point-and-mark
bind -m vi-insert '\c-u':unix-line-discard
bind -m vi-insert '\c-w':unix-word-rubout
bind -m vi-insert '\c-k':kill-line

function comm_put_ser() 
{
    user=$1
    ip_fix=$2
    port=$3
    passwd=$4
    sshpass -p"$passwd" scp -v -q -2 -P $port $6 $user@$ip_fix.$5:~/
}

function comm_get_ser() 
{
    user=$1
    ip_fix=$2
    port=$3
    passwd=$4
    sshpass -p"$passwd" scp -v -q -2 -P $port  $user@$ip_fix.$5:~/$6 . 
}

function alias_comm_cmd() 
{
    flag=$2
    user=$3
    ip_fix=$4
    port=$5
    passwd=$6
    user_flag=$7
    eval "alias c$flag$1$user_flag=' sshpass -p\"$passwd\" ssh -q -2 -p$port $user@$ip_fix.$1' "
    eval "alias put$flag$1$user_flag=' comm_put_ser $user $ip_fix $port \"$passwd\" $1 ' "
    eval "alias get$flag$1$user_flag=' comm_get_ser $user $ip_fix $port \"$passwd\" $1 ' "
}

#alias_comm_cmd 39 '' dingqisong 172.16.56 22 123456

eval `dircolors ~/.dircolors`
#export PATH=$PATH:/usr/local/bin
#export PATH=/usr/lib64/qt-3.3/bin:/usr/local/bin:/bin:/usr/bin:/usr/local/sbin:/usr/sbin:/sbin:$HOME/protobuf/bin
#export LD_LIBRARY_PATH=$HOME/protobuf/lib:$HOME/mysql++/lib
export LD_LIBRARY_PATH=/usr/local/lib
#export CPLUS_INCLUDE_PATH=$HOME/server/res/entities
