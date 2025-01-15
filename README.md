# win_console_test

## 概要
- Windows Terminal で、入出力を確認する C のテストプログラムです。  
  (Windows Terminal でしか動作しないため、Windows 10 以後が必要です)


## インストール方法
1. MSYS2/MinGW-w64 (64bit) のインストール  
   事前に MSYS2/MinGW-w64 (64bit) がインストールされている必要があります。  
   以下のページを参考に、開発環境のインストールを実施ください。  
   https://gist.github.com/Hamayama/eb4b4824ada3ac71beee0c9bb5fa546d  
   (すでにインストール済みであれば本手順は不要です)  
   (記事が古いので、現状と合っていないかもしれない)

2. 本プログラムのコンパイル  
   0000_compile.bat を (ダブルクリック等で) 実行してください。

3. 本プログラムの実行  
   1000_exec_input.bat  で、入力確認のテストプログラムを起動します。  
   1001_exec_output.bat で、出力確認のテストプログラムを起動します。


## 実行結果
1. 入力確認のテストプログラム  
   ![image](image/win_con_input_0001.png)


2. 出力確認のテストプログラム  
   ![image](image/win_con_output_0001.png)  
   ![image](image/win_con_output_0002.png)


## ノウハウ等
1. win_con_lib_1000.h にコンソール入出力の関数を定義しています。  
   24-bit カラーと属性 (bold, italic, underline, reverse) の設定は、  
   VT エスケープシーケンスで行っています。  
   ( https://learn.microsoft.com/ja-jp/windows/console/console-virtual-terminal-sequences )


## TODO
1. DLL化


## 環境等
- OS
  - Windows 10 (version 22H2) (64bit)
- 環境
  - MSYS2/MinGW-w64 (64bit) (gcc version 14.2.0 (Rev2, Built by MSYS2 project))


## 履歴
- 2025-1-16 v1.00 (初版)


(2025-1-16)
