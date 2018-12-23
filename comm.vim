"---------------------------------------------------------------------------
"add to   .vimrc
"source ~/.vim/comm.vim
"---------------------------------------------------------------------------

"set nocompatible

"Brief help
":BundleList          - list configured bundles
":BundleInstall(!)    - install(update) bundles
":BundleSearch(!) foo - search(or refresh cache first) for foo
":BundleClean(!)      - confirm(or auto-approve) removal of unused bundles

"Vundle 管理
set rtp+=~/.vim/bundle/vundle/
call vundle#rc()

" 让 Vundle 管理 Vundle
" 此条必须有
Bundle 'gmarik/vundle'

" 代码源在 github 上的
Bundle 'vim-scripts/DoxygenToolkit.vim'
Bundle 'vim-scripts/FencView.vim'
Bundle 'vim-scripts/OmniCppComplete'
Bundle 'vim-scripts/Rainbow-Parentheses-Improved-and2'
Bundle 'vim-scripts/Tagbar'
Bundle 'vim-scripts/matchit.zip'
Bundle 'vim-scripts/python.vim'
Bundle 'vim-scripts/xml.vim'
Bundle 'vim-scripts/YankRing.vim'
Bundle 'vim-scripts/auto_mkdir'
Bundle 'vim-scripts/Tabular'
Bundle 'vim-scripts/AutoClose'
Bundle 'vim-scripts/a.vim'
Bundle 'vim-scripts/comments.vim'
Bundle 'vim-scripts/ack.vim'
Bundle 'Lokaltog/vim-powerline'
Bundle 'Lokaltog/vim-easymotion'
Bundle 'Shougo/unite.vim'
Bundle 'yueyoum/vim-alignment'
Bundle 'tpope/vim-surround'
Bundle 'bootleq/vim-cycle'
Bundle 'kana/vim-smartword'
Bundle 'altercation/vim-colors-solarized'
Bundle 'mbbill/echofunc'
"Bundle 'Valloric/YouCompleteMe'
Bundle 'scrooloose/nerdtree'
Bundle 'SirVer/ultisnips'
Bundle 'honza/vim-snippets'
"Bundle 'davidhalter/jedi-vim'
 
"---------------------------------------------------------------------------
"GENERAL SET
"---------------------------------------------------------------------------
set vb t_vb=
"Sets how many lines of history VIM har to remember
set history=400

let mapleader =","

" Enable filetype plugins
filetype plugin indent on
"语法
syntax enable on

"备份
set backup
"备份目录 
if !isdirectory($HOME."/.vim/backupdir")
    silent! execute "!mkdir ~/.vim/backupdir"
endif
set backupdir=~/.vim/backupdir
"不产生.swap文件
set noswapfile

"中文帮助
set helplang=cn

"disable alt
set winaltkeys=no
"tags 位置
"set tags=~/.vim/cpp_tags,~/.vim/comm_tags,tags
set tags=$HOME/mf/shyloo/src/tags,$HOME/g48/ena/tags

"折叠
set foldmethod=syntax
""默认情况下不折叠
set foldlevel=99

"php补全
autocmd FileType php set omnifunc=phpcomplete#CompletePHP

"protobuf
augroup filetype
   au! BufRead,BufNewFile *.proto setfiletype proto
augroup end

"重新打开时自动定位到原来的位置
autocmd BufReadPost *
	\ if line("'\"") > 0 && line ("'\"") <= line("$") |
	\   exe "normal! g'\"" |
	\ endif
"转换paste
autocmd InsertLeave * if &paste == 1|set nopaste |endif

" ENCODING SETTINGS
let &termencoding = &encoding
set encoding=utf-8
set termencoding=utf-8
set fileencoding=utf-8
set fileencodings=ucs-bom,utf-8,gb2312,big5,euc-jp,euc-kr,latin1
set enc=utf-8
set fencs=utf-8,gbk,gb2312,gb18030
language messages POSIX

"自动更新cpp修改时间
"autocmd BufWritePre,FileWritePre *.cpp,*.c,*.h,*.hpp exec "normal ms"|call LastModified()|exec "normal `s"

"for cmake ':make' ,由于定位错误,中文会有问题，如下调整
if finddir("build") == "build"
    set makeprg=export\ LANG=zh_CN:en;make\ -C\ ./build
endif

set scrolloff=5 " 距离垂直边界 n 行就开始滚动
set sidescroll=1 " 水平滚动列数
set sidescrolloff=10 " 距离水平边界 n 行就开始滚动

"Favorite filetypes
set fileformats=unix,mac,dos

if has('gui_running')
    ""set guifont=Monospace\ for\ Powerline\ 14
    set guifont=Monospace\ 12
endif

"主题 
"colorscheme desert
let g:solarized_termcolors=256
let g:solarized_termtrans=1
let g:solarized_hitrail   =   0
set background="dark"
colorscheme solarized
set cursorline

"显示命令
set showcmd

"Include search
set incsearch
"highlight search
set hlsearch

" For regular expressions turn magic on
set magic

" Show matching brackets when text indicator is over them
set showmatch
set matchpairs+=<:>

"鼠标和剪贴板
set mouse=v
set clipboard+=unnamed
" Height of the command bar
set cmdheight=1

" Wildmenu completion , Cool tab completion stuff{
set wildmenu
set wildmode=list:longest,full
" Ignore compiled files
set wildignore+=.hg,.git,.svn                    " Version control
set wildignore+=*.aux,*.out,*.toc                " LaTeX intermediate files
set wildignore+=*.jpg,*.bmp,*.gif,*.png,*.jpeg   " binary images
set wildignore+=*.o,*.obj,*.exe,*.dll,*.manifest " compiled object files
set wildignore+=*.spl                            " compiled spelling word lists
set wildignore+=*.sw?                            " Vim swap files
set wildignore+=*.DS_Store                       " OSX bullshit
set wildignore+=*.luac                           " Lua byte code
set wildignore+=migrations                       " Django migrations
set wildignore+=*.pyc                            " Python byte code
set wildignore+=*.orig                           " Merge resolution files
set wildignore+=classes
set wildignore+=lib
set wildignore=*.o,*~,*.pyc
" }

"Always show current position
set ruler

" enables automatic C program indenting
set autoindent
set smartindent 
set cindent 
set smarttab
"set expandtab " 开启把Tab扩展为空格
set tabstop=4 " 设置Tab宽度为4个字符
set softtabstop=4 " 软缩进宽度
set shiftwidth=4 " 设置移动代码块宽度
set shiftround " 移动代码块时取整到缩进宽度

" autoread when a file is changed from the outside
set autoread
" write buffer when leaving
set autowrite
" Don't redraw while executing macros (good performance config)
set lazyredraw
"退格键能删除
set backspace=indent,eol,start
"backspace and cursor keys wrap to
set whichwrap+=<,>

" No annoying sound on errors
set noerrorbells
set novisualbell
set t_vb=
set tm=500

" Highlight VCS conflict markers
syn match ErrorMsg '^\(<\|=\|>\)\{7}\([^=].\+\)\?$'

"vimtips 
command! -nargs=0 VIMTIPS  :tabe | :r ! w3m -dump http://zzapper.co.uk/vimtips.html 

"----------------------------------------------------------------------------
"MAP BIND
"----------------------------------------------------------------------------

"use jj replace esc 
inoremap jj <Esc>

nnoremap gr gT

map  <F1> :help <C-R>=expand('<cword>')<CR><CR>

" }}} 跨 Vim 剪貼 {{{2
" http://vim.wikia.com/wiki/Transfer_text_between_two_Vim_instances
nmap \p :r $HOME/.vimxfer<CR>
vmap \c :w! $HOME/.vimxfer<CR>

nnoremap ,q <Esc>:q!<CR>
"nnoremap ,w :w!<CR>:nohl<CR>
nnoremap ,w <C-w>w

nnoremap <F6> :nohl<CR>

" sudo write this
nnoremap ,W <Esc>:w !sudo tee % >/dev/null<CR>
nnoremap ,e <Esc>:e 
nnoremap ,x <Esc>:!
nmap  Y  y$

"cmd model map
cnoremap <C-A> <HOME>
cnoremap <C-E> <END>
cnoremap <C-D> <DELETE>
cnoremap <C-B> <LEFT>
cnoremap <C-F> <RIGHT>
cnoremap <C-P> <UP>
cnoremap <C-N> <DOWN>

"tabedit
"nnoremap ,t <Esc>:tabedit 
"nnoremap <C-p> <C-PageUp>
"nnoremap <C-n> <C-PageDown>

"查找当前光标下的单词
nnoremap ,g :call P_grep_curword()<CR>
vnoremap ,g :call VisualSelection('gv')<CR>
"nnoremap ,r <Esc>:call RESET_CTAG_CSCOPE()<CR>
nnoremap ,m <Esc>:make<CR><CR>
nnoremap ,y <Esc>:call OPT_RANGE("ya")<CR>
nnoremap ,Y <Esc>:call OPT_RANGE("yi")<CR>
nnoremap ,d <Esc>:call OPT_RANGE("da")<CR>
nnoremap ,D <Esc>:call OPT_RANGE("di")<CR>
"转换单词大小写
nnoremap ,u <Esc>:call SET_UAW()<CR>

"支持粘贴
inoremap kk <Esc>:set paste<CR>i

"窗口间移动
nnoremap \i  [I
nnoremap <C-H> <Esc><C-W>h
nnoremap <C-L> <Esc><C-W>l
nnoremap <C-J> <Esc><C-W>j
nnoremap <C-K> <Esc><C-W>k

"Fast editing of .vimrc
nnoremap \v <ESC>:e! ~/.vim/comm.vim<cr>
"Switch to current dir
"nnoremap ,cd <ESC>:cd %:p:h<cr>

"在正常模式下的整块移动
"大括号内向左移
nmap <F7> <Esc><i{
"大括号内向右移
nmap <F8> <Esc>>i{
"选择区移动
vnoremap <F7> <gv
vnoremap <F8> >gv
" Visual mode pressing * or # searches for the current selection
" Super useful! From an idea by Michael Naumann
vnoremap <silent> * :call VisualSelection('f')<CR>
vnoremap <silent> # :call VisualSelection('b')<CR>

"quick fix toggle
"nnoremap <F4> <Esc>:call ToggleQF()<CR>
nnoremap ,cn <Esc>:cn<CR>
nnoremap ,cp <Esc>:cp<CR>
"---------------------------------------------------------------------------
"插件设置
"---------------------------------------------------------------------------
"ack.vim{
set grepprg=/user/bin/ack-grep
let g:ackprg="/usr/bin/ack-grep -H --nocolor --nogroup"
"}

"vim-smartword{{{
nmap w  <Plug>(smartword-w)
nmap b  <Plug>(smartword-b)
nmap e  <Plug>(smartword-e)
nmap ge  <Plug>(smartword-ge)
"}}}

"autoclose{{{
nmap <F2> <Plug>ToggleAutoCloseMappings
"}}}}

"a.vim {{{
nnoremap ,a <Esc>:A<CR>
"}}}

"fuzzyfinder {{{
nnoremap \ff :FufFile<CR>
nnoremap \fb :FufBuffer<CR>
nnoremap \ft :FufTag<CR>
"}}}

"unite{{{
nnoremap ,f :Unite file<CR>
nnoremap ,b :Unite buffer<CR>
""}}}

"tabular{{{
nnoremap ,= :call SetAlign()<CR>
"}}}

"vim-easymotion{{{
let g:EasyMotion_leader_key = '0'
nmap \w <Esc>00w
"}}}

"FencView {{{
let g:fencview_autodetect = 1                      
"}}}

"YankRing {{{
let g:yankring_max_history = 10
let g:yankring_min_element_length = 3
""let g:yankring_max_display = 50 
let g:yankring_persist = 0
nnoremap <silent> <C-Y> :YRShow<CR> 
let g:yankring_replace_n_pkey = '<m-p>'
let g:yankring_replace_n_nkey = '<m-n>'
let g:yankring_history_dir = '~/.vim/'
let g:yankring_history_file='.yankring_history_file'
"}}}

" omnicppcomplete{{{
"用于支持代码补全时，提示存在。
set complete=.,w,b,u,t
set completeopt=longest,menuone
"当离开INSERT模式时，Preview窗口会自动关闭
"autocmd CursorMovedI * if pumvisible() == 0|pclose|endif
"autocmd InsertLeave * if pumvisible() == 0|pclose|endif
inoremap <expr><CR> pumvisible() ?"\<C-Y>" : "\<c-g>u\<cr>"
inoremap <expr><C-U>  pumvisible()?"\<C-E>":"\<C-U>"
"根据标签补全
inoremap <C-]> <C-X><C-]> 
"补全文件名
inoremap <C-F> <C-X><C-F> 
"补全宏定义
inoremap <C-D> <C-X><C-D>
"整行补全
inoremap <C-L> <C-X><C-L>
"根据头文件内关键字补全
inoremap <C-I> <C-X><C-I> 
"用户自定义补全方式   
inoremap <C-U> <C-X><C-U> 
"全能补全
inoremap <C-O> <C-X><C-O>
" 用于支持 退格后 . -> 代码补全
"inoremap   <expr> <Backspace>  Ex_bspace() 
"let OmniCpp_ShowScopeInAbbr = 1
""支持STL模板
"let OmniCpp_DefaultNamespaces   = ["std", "_GLIBCXX_STD"]
""不自动选择第一个
"let OmniCpp_SelectFirstItem = 0
""使用本地搜索函数
let OmniCpp_LocalSearchDecl = 1
""::补全
let OmniCpp_MayCompleteScope =1
" }}}


" cscope setting {{{
if has("cscope")
    set csprg=/usr/bin/cscope
    set csto=0
    set cst
    set nocsverb
    " add any database in current directory
    if filereadable("cscope.out")
        silent cs add cscope.out
    endif
    set csverb
endif
nnoremap ,s :cs find s <C-R>=expand("<cword>")<CR><CR>
vnoremap ,s :call  VisualSelection('cs')<CR>
" }}}

"powerline{{{ 状态栏
let g:Powerline_colorscheme = 'solarized256'
set laststatus=2
set t_Co=256
"}}}

"Doxygen插件{{{
let g:DoxygenToolkit_briefTag_pre="@brief  " 
let g:DoxygenToolkit_paramTag_pre="@param  " 
let g:DoxygenToolkit_returnTag="@return  " 
let g:DoxygenToolkit_authorName="seasondi" 
let g:DoxygenToolkit_undocTag="DOXIGEN_SKIP_BLOCK"
let g:DoxygenToolkit_briefTag_funcName = "no"
let g:DoxygenToolkit_maxFunctionProtoLines = 30
nnoremap \d :Dox<CR>
""nnoremap \da :DoxAuthor<CR>
"}}}

"rainbow_parenthsis_options.vimbow {{{
let g:rainbow_ctermfgs = [ 'darkgray', 'darkblue' ,'magenta','darkgreen', 'cyan', 'darkred', ]
let g:rainbow_active = 0
let g:rainbow_operators = 0
""nnoremap <F5>  :call rainbow#load()<CR>
"autocmd BufEnter *.cpp,*hpp,*.h,*.c  call rainbow#load()
"}}}


" Tagbar setting{{{
let g:tagbar_width = 30 
let g:tagbar_expand = 0
let g:tagbar_autofocus = 1
"nnoremap <silent> <F3> <Esc>:TagbarToggle<cr>
" }}}

"vim_multi_cursor{{{
let g:multi_cursor_use_default_mapping=0
" Default mapping
let g:multi_cursor_next_key='<C-n>'
let g:multi_cursor_prev_key='<C-p>'
let g:multi_cursor_skip_key='<C-z>'
let g:multi_cursor_quit_key='<Esc>'
"}}}

"xml.vim{{{
let xml_use_xhtml = 1
let html_use_css = 1
let html_number_lines = 0
let use_xhtml = 1
"}}}

"cycle.vim{{{
let g:cycle_no_mappings = 1
let g:cycle_max_conflict = 7
let g:cycle_phased_search = 1
nmap <silent> \a <Plug>CycleNext
vmap <silent> \a <Plug>CycleNext

let g:cycle_default_groups = [
\ [['true', 'false']],
\ [['yes', 'no']],
\ [['on', 'off']],
\ [['+', '-']],
\ [['>', '<']],
\ [['"', "'"]],
\ [['==', '!=']],
\ [['0', '1']],
\ [['and', 'or']],
\ [['in', 'out']],
\ [['cs', 'sc']],
\ [['up', 'down']],
\ [['min', 'max']],
\ [['get', 'set']],
\ [['add', 'remove']],
\ [['to', 'from']],
\ [['read', 'write']],
\ [['save', 'load', 'restore']],
\ [['next', 'previous', 'prev']],
\ [['only', 'except']],
\ [['without', 'with']],
\ [['exclude', 'include']],
\ [['width', 'height']],
\ [['asc', 'desc']],
\ [['start', 'end']],
\ [['是', '否']],
\ [['上', '下']],
\ [['左', '右']],
\ [['前', '后']],
\ [['內', '外']],
\ [['男', '女']],
\ [['east', 'west']],
\ [['south', 'north']],
\ [['prefix', 'suffix']],
\ [['decode', 'encode']],
\ [['short', 'long']],
\ [['pop', 'shift']],
\ [['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday',
\ 'Friday', 'Saturday'], ['hard_case', {'name': 'Days'}]],
\ [['{:}', '[:]', '(:)'], 'sub_pairs'],
\ [['（:）', '「:」', '『:』'], 'sub_pairs'],
\ ]
"}}}

"----------------------------------------------------------------------------
" FileType related
"----------------------------------------------------------------------------

""""""""""""
"general
""""""""""""
autocmd BufEnter *  set tabstop=4 
" use syntax complete if nothing else available
if has("autocmd") && exists("+omnifunc")
    autocmd Filetype *
        \ if &omnifunc == "" |
        \ setlocal omnifunc=syntaxcomplete#Complete |
        \ endif
endif

""""""""""""
"c c++
""""""""""""
autocmd BufEnter  *.cpp,*.c,*.h call s:SET_PATH("include") 
autocmd FileType c set omnifunc=ccomplete#Complete
".c  .h 文件设为 .cpp
autocmd BufEnter *.c  set filetype=cpp
autocmd BufEnter *.h  set filetype=cpp

""""""""""""
"php
""""""""""""
autocmd BufEnter  *.php call s:SET_PATH("pub") 

""""""""""""
"python
""""""""""""
autocmd FileType python setlocal omnifunc=pythoncomplete#Complete
autocmd BufRead,BufNewFile *.py set ai
autocmd BufRead *.py set smartindent noexpandtab cinwords=if,elif,else,for,while,try,except,finally,def,class
autocmd FileType python setlocal et sta sw=4 sts=4
autocmd FileType python setlocal foldmethod=indent
autocmd FileType python set complete+=k~/.vim/syntax/python.vim "isk+=.,(
function HeaderPython()
    call setline(1, "# -*- coding: utf-8 -*-")
    normal G
    normal o
    normal o
endf
autocmd BufNewFile *.py call HeaderPython()

""""""""""""
" HTML 
""""""""""""
au FileType html set ft=xml
au FileType html set syntax=html
autocmd FileType html,markdown setlocal omnifunc=htmlcomplete#CompleteTags
autocmd FileType xml setlocal omnifunc=xmlcomplete#CompleteTags
autocmd BufRead *.mxml set filetype=mxml

""""""""""""
" Vim 
""""""""""""
autocmd FileType vim set nofen
autocmd FileType vim map <buffer> <leader><space> :w!<cr>:source %<cr>

""""""""""""
"others
""""""""""""
" Enable omni completion. (Ctrl-X Ctrl-O)
autocmd FileType javascript setlocal omnifunc=javascriptcomplete#CompleteJS
autocmd FileType css set omnifunc=csscomplete#CompleteCSS
autocmd FileType java set omnifunc=javacomplete#Complete
autocmd BufRead *.as set filetype=actionscript
autocmd BufRead \d\+-\(\w\+\)-\d\{6\}-\d\{4\}  set filetype=log
"----------------------------------------------------------------------------
" FUNCTIONS
"----------------------------------------------------------------------------
function! SetAlign()
    let ch=getline(line('.'))[col('.')-1]
    let next=getline(line('.'))[col('.')]
    if ch=='/' and next=='/' 
        let ch='\/\/'
    else if ch=='+' and next=='='
        let ch = ch . next
    else if ch == next
        let ch = ch . next
    endif
    exec "Tabularize /" . ch
endfunc

"quickfix 开关 
function! ToggleQF()
    if !exists("g:fx_toggle")
        let g:fx_toggle = 0
    endif
    if g:fx_toggle == 0
        let g:fx_toggle = 1
        copen
    else
        let g:fx_toggle = 0
        cclose
    endif
endfunc

" Visual mode related
function! VisualSelection(direction) range
    let l:saved_reg = @"
    execute "normal! vgvy"
    
    let l:pattern = escape(@", '\\/.*$^~[]')
    let l:pattern = substitute(l:pattern, "\n$", "", "")
    
    if a:direction == 'b'
        execute "normal ?" . l:pattern . "^M"
    elseif a:direction == 'gv'
        exec "Ack " . l:pattern
    elseif a:direction == 'replace'
        ""call CmdLine("%s" . '/'. l:pattern . '/')
    elseif a:direction == 'f'
        execute "normal /" . l:pattern . "^M"
    elseif a:direction == 'cs'
        execute "cs find s " . l:pattern 
    elseif a:direction == 'oi'
        execute "!onlinei " . l:pattern 
    elseif a:direction == 'oo'
        execute "!onlineo " . l:pattern 
    endif
    let @/ = l:pattern
    let @" = l:saved_reg
endfunction

"自动更新 修改时间
function! LastModified()
if search("\\/\\*LastModified: \\d\\{4}-\\d\\{2}-\\d\\{2} \\d\\{2}:\\d\\{2}:\\d\\{2}\\*\\/","","")>0
    exe "silent! $,$g/$/s/LastModified: .*/LastModified: " .
        \ strftime("%Y-%m-%d %H:%M:%S") . "\\*\\/"
else
    exe "silent! $,$g/$/s/$/\r\\/\\*LastModified: " .
        \ strftime("%Y-%m-%d %H:%M:%S") . "\\*\\/"
endif

"退格时自动补全
function! Ex_bspace()
    if (&filetype == "cpp" || &filetype == "c" )
        let pre_str= strpart(getline('.'),0,col('.')-2)
        if pre_str  =~ "[.][ \t]*$" || pre_str  =~ "->[ \t]*$"   
            return "\<Backspace>\<C-X>\<C-O>"	
        endif 
    endif

    if (&filetype == "python"|| &filetype == "html"  || &filetype == "python"  )
        let pre_str= strpart(getline('.'),0,col('.')-2)
        if pre_str  =~ "[.][ \t]*$"
            return "\<Backspace>\<C-X>\<C-O>\<C-P>\<C-R>=pumvisible() ? \"\\<down>\" : \"\"\<cr>"	
        endif 
    endif

    "default
    return "\<Backspace>"	
endf


endfunc

"获取当前路径的上一级的路径
function! GET_UP_PATH(dir)
    let pos=len(a:dir)-1
    while pos>0 
        if (a:dir[pos]=="/" )
            return 	strpart(a:dir,0,pos)
        endif
        let pos=pos-1 
    endwhile
    return  ""  
endfunction

"设置相关tags
function! s:SET_TAGS()
    let dir =getcwd()  "获得源文件路径
    set tags=
    "在路径上递归向上查找tags文件 
    while dir!=""
        if findfile("tags",dir ) !=""
            "找到了就加入到tags
            exec "set tags+=" . dir . "/tags"
        endif
        "得到上级路径
        let dir=GET_UP_PATH(dir)
    endwhile
    if ( &filetype =="cpp" )
        set tags+=~/.vim/bundle/myfix/comm_tags
    endif
endfunction

"设置相关 include , for cmd : gf 
function! s:SET_PATH( find_dir )
    let dir = expand("%:p:h") "获得源文件路径
    let dir_relative=''
    let g:alternateSearchPath = ''
    "let g:alternateSearchPath = 'sfr:../source,sfr:../src,sfr:../include,sfr:../inc,sfr:.'
    "在路径上递归向上查找tags文件 
    while dir!=""
        if finddir(a:find_dir ,dir ) !=""
            "找到了就加入到tags
            exec "set path+=" . dir . "/". a:find_dir
            let g:alternateSearchPath = g:alternateSearchPath.'sfr:'.dir_relative.a:find_dir."," 
        endif
        "得到上级路径
        let dir_relative=dir_relative . "../"
        let dir=GET_UP_PATH(dir)
    endwhile
endfunction

"upper case
function! SET_UAW()
    let save_cursor = getpos(".")
    
    let line = getline('.')
    let col_num = col('.')
    if match("ABCDEFGHIJKLMNOPQRSTUVWXYZ",line[col_num-1])!= -1
        exec "normal! guaw"
    else
        exec "normal! gUaw"
    endif
    
    call setpos('.', save_cursor)
endfunction

"得到光标下的单词
function! P_grep_curword() 
    let curword=expand("<cword>")
    exec "Ack " . curword
endfunction

"重新生成ctag cscope
function! RESET_CTAG_CSCOPE() 
    "if(executable('cscope') && has("cscope") )
        "silent! execute "!find . -name '[^.]*.h' -o -name '[^.]*.c' -o -name '[^.]*.cpp' -o -name '[^.]*.hpp' -o -name '[^.]*.cc' > cscope.files"
        "silent! execute "!cscope -bkq"
        "if (filereadable("cscope.out"))
            "execute "cs reset"
        "endif
    "endif
    if(executable('ctags'))
        silent! execute "!rm -f tags"
        silent! execute "!ctags -R  --languages=c,c++ --c++-kinds=+p --fields=+iaS --extra=+q ."
    endif
    exec "redraw!"
endfunction

function! OPT_RANGE( opt_str ) 
    let cur_char=getline('.')[col('.') - 1] 
    if cur_char == "(" || cur_char == "<" || cur_char == "{" || cur_char == "[" || cur_char == "\"" || cur_char == "'" || cur_char == ")" || cur_char == ">" || cur_char == "}" || cur_char == "]" 
        exec "normal! ".a:opt_str.cur_char
    endif
endfunction


" 在视图模式下的整块移动
function! SET_BLOCK_MOVE_V( move_type )
    if a:move_type==0
        exec "'<,'>s/^/    /"
    else
        exec "'<,'>s/^    //"
    endif
    let linecount = line("'>") - line("'<")
    let save_cursor_begin = getpos("'<")
    call setpos('.', save_cursor_begin)
    exec  "normal! v" . linecount . "j"	
endfunction

"得到光标下的单词
function! GetCurWord()
	return expand("<cword>")
endfunc

"YouCompleteMe
"{{
let g:ycm_global_ycm_extra_conf = '~/.vim/bundle/YouCompleteMe/third_party/ycmd/cpp/ycm/.ycm_extra_conf.py'
let g:ycm_confirm_extra_conf=0
let g:ycm_collect_identifiers_from_tag_files = 1
let g:ycm_seed_identifiers_with_syntax=1
let g:ycm_key_list_select_completion = ['<c-n>', '<Down>']
let g:ycm_key_list_previous_completion = ['<c-p>', '<Up>']
"nnoremap <leader>d :YcmCompleter GoToDefinitionElseDeclaration<CR>
nnoremap <leader>d :YcmCompleter GoTo<CR>
nnoremap <F3> :YcmCompleter GoToDefinition<CR>
nnoremap <F4> :YcmCompleter GoToDeclaration<CR>
nnoremap <leader>r :YcmCompleter GoToReferences<CR>
nnoremap <leader>z :YcmDiags<CR>
"}}

"NERDTree
"{{
"autocmd vimenter * NERDTree
"let g:NERDTreeWinPos=1

autocmd StdinReadPre * let s:std_in=1
autocmd VimEnter * if argc() == 0 && !exists("s:std_in") | NERDTree | endif
nnoremap <leader>f :NERDTreeToggle<CR>
autocmd bufenter * if(winnr("$") == 1 && exists("b:NERDTree") && b:NERDTree.isTabTree()) | q | endif
let g:NERDTreeDirArrowExpandable = '▸'
let g:NERDTreeDirArrowCollapsible = '▾'
"}}

"taglist
"{{
let Tlist_Inc_Winwidth=0
"let Tlist_Use_Right_Window=1
let Tlist_Process_File_Always=1
let Tlist_GainFocus_On_ToggleOpen=1
let Tlist_File_Fold_Auto_Close=1
let Tlist_Show_Menu=1
let Tlist_Show_One_File=1
nnoremap <leader>t :TlistToggle<CR>
"}}

"ultisnips
"{{
" Trigger configuration. Do not use <tab> if you use https://github.com/Valloric/YouCompleteMe.
let g:UltiSnipsExpandTrigger="<tab>"
let g:UltiSnipsJumpForwardTrigger="<c-b>"
let g:UltiSnipsJumpBackwardTrigger="<c-z>"

"" If you want :UltiSnipsEdit to split your window.
let g:UltiSnipsEditSplit="vertical"
"}}
