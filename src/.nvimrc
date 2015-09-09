let &makeprg='cd ../proj/ && cmake -GNinja . && ninja gameenginecourse'
" Compile and run
inoremap <F29> <Esc>:wa<CR>:Neomake!<CR> :!cd ../bin/linux\ Clang/Debug/ && ./gameenginecourse<CR>
noremap <F29> :wa<CR>:Neomake!<CR> :!cd ../bin/linux\ Clang/Debug/ && ./gameenginecourse<CR>
inoremap <F5> <Esc>:wa<CR>:!cd ../proj && cmake -G "Ninja" . && ninja gameenginecourse && cd ../bin/linux\ Clang/Debug/ && ./gameenginecourse<CR>
noremap <F5> :wa<CR>:!cd ../proj && cmake -G "Ninja" . && ninja gameenginecourse && cd ../bin/linux\ Clang/Debug && ./gameenginecourse<CR>
" Compile
inoremap <F31> <Esc>:wa<CR>:Neomake!<CR>
noremap <F31> :wa<CR>:Neomake!<CR>
inoremap <F7> <Esc>:wa<CR>:!cd ../proj/ && cmake -G "Ninja" . && ninja gameenginecourse<CR>
noremap <F7> :wa<CR>:!cd ../proj/ && cmake -G "Ninja" . && ninja gameenginecourse<CR>

command MS mks! ~/.vim/sessions/gameenginecourse.vim

let g:formatdef_my_custom_cpp = '"astyle --options=.codestyle.astyle"'
let g:formatters_cpp = ['my_custom_cpp']

let g:formatdef_my_custom_h = "astyle --options=.codestyle.astyle"
let g:formatters_h = "my_custom_h"

let g:formatdef_my_custom_hpp = "astyle --options=.codestyle.astyle"
let g:formatters_hpp = "my_custom_hpp"
