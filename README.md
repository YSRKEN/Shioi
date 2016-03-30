# Shioi

## Abstract
Shioi is renju player and solver by C++ and C#.

## How to use
- [File] "Clear board", "Open file", "Save file", "Close software"
- [Edit] Notification : The format of move's text is used by "Date Gomoku".
- [Game] You can seek moves of the game.
- [Help] About of this Software
- You can drag and drop file. Then, this software open this file.

## File Format
### *.ban
This format is used by "Gomoku Kyousou"(五目競争) which is a computer renju tournament.  
http://hp.vector.co.jp/authors/VA009422/5guide0.txt

### *.dt5
This format is used by "Date Gomoku"(伊達五目).  
But, this is only saved in clipboard.  
http://www.vector.co.jp/soft/win95/game/se067096.html

### *.lws
This format is used by "Fiver6".  
http://nosovsky.narod.ru/index21.html

### *.rnj
This format is used by "Renju Kozou"(連珠小僧).  
http://homepage3.nifty.com/SGL/RENJU/

### *.kif
This format is used by "MonteCarlo Gomokunarabe"(モンテカルロ五目並べ).  
http://www.vector.co.jp/soft/winnt/game/se502329.html

### *.nrf
This format is used by "Niroi Renju hu"(にろい連珠譜).  
http://www.vector.co.jp/soft/win95/game/se169850.html

## Version History
|Version|Information|
|-------|-----------|
|0.3.0|Add support of Renju's rule(include of "Ina-San-San")|
|0.2.0|drag and drop support, \*.kif and \*.nrf support, some bug fix|
|0.1.0|first release|

# Shioi(しおい)

## 概要
Shioiは、C++とC#によって書かれた連珠ソフトです。しおいって呼んでね。

## 使い方
- 「File」……上から、「盤面初期化」・「ファイルを開く」・「ファイル保存」・「終了」です。
- 「Edit」……上から、「手順を伊達五目形式でコピー」「手順を伊達五目形式で貼り付け」「盤面をテキストでコピー」「盤面を画像でコピー」です。
- 「Game」……上から、「一手進める」「一手戻る」「最初に戻す」「最後まで飛ぶ」になります。
- 「Help」……ソフトウェアについての情報が確認できます。
- 棋譜ファイルをドラッグ＆ドロップして読み込めます。

## ファイル形式
### *.ban
この形式は、五目競争という大昔のコンピュータ連珠大会に使用されていたものです。  
http://hp.vector.co.jp/authors/VA009422/5guide0.txt

### *.dt5
この形式は、伊達五目というソフトウェアで使用しているものです。  
ただし、ファイルとして保存できるわけではなく、クリップボードにコピー・貼り付けするための形式です。  
したがって、この拡張子はShioi用にでっち上げました。  
(ただし、貼り付ける際に伊達五目が何故かフリーズするのは伊達五目のバグだと思われます)  
http://www.vector.co.jp/soft/win95/game/se067096.html

### *.lws
この形式は、Fiver6というソフトウェアで使用しているものです。  
http://nosovsky.narod.ru/index21.html

### *.rnj
この形式は、連珠小僧というソフトウェアで使用しているものです。  
http://homepage3.nifty.com/SGL/RENJU/

### *.kif
この形式は、モンテカルロ五目並べというソフトウェアで使用しているものです。  
http://www.vector.co.jp/soft/winnt/game/se502329.html

### *.nrf
この形式は、にろい連珠譜というソフトウェアで使用しているものです。  
http://www.vector.co.jp/soft/win95/game/se169850.html

## 更新履歴
|Version|Information|
|-------|-----------|
|0.3.0|盤面における、各種追い手および禁手表示を実装(否三々対応済)|
|0.2.0|ドラッグ＆ドロップに対応、\*.kifと\*.nrfに対応、幾つかのバグ修正|
|0.1.0|最初のリリース|
