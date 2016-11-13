# dbskkd-cdb Release Notes in Japanese

for version dbskkd-cdb-3.00

by Kenji Rikitake <mailto:kenji.rikitake@acm.org>

last updated 13-NOV-2016

## dbskkd-cdb の概要

dbskkd-cdb は， skkserv と同様の機能を提供する SKK 用の共通辞書サーバーである．辞書は D. J. Bernstein の cdb を使ってデータベース化し，検索を高速化している．本サーバーは同種のツール pskkserv を基にしている．

cdb <http://cr.yp.to/cdb.html>

本プログラムは cdb 互換ライブラリである TinyCDB へのリンクを前提としている．ビルドには TinyCDB のヘッダ cdb.h とライブラリ libcdb.a が必要である．(cdbの同種のライブラリも使用可能であるが，テストしていない．)

TinyCDB <http://www.corpit.ru/mjt/tinycdb.html>

## dbskkd-cdb の動作環境

dbskkd-cdb は，現時点では FreeBSD 11.0-STABLE (svn r308241) にて基本的動作を確認している．

FreeBSD の port および debian の package については動作を確認している．

他の OS でも大きな変更なしに動作すると思われる．ただし動作の確認は動作の保証を意味しない．また動作条件の細かな変化による誤動作については一切保証しない．

## インストール前の注意

1. 添付の makeskkcdb.sh を実行する．このスクリプトでは辞書ファイルは skktocdbm.sh の標準入力に与える． ファイル名の既定値は /usr/local/share/skk/SKK-JISYO.L である．実行するとカレントディレクトリに SKK-JISYO.L.cdb が作られる．この辞書ファイルはインストールの際 dbskkd-cdb.c 中の JISYO\_FILE で指定するディレクトリに置く．JISYO\_FILE の既定値は /usr/local/share/skk/SKK-JISYO.L.cdb である． 注意: 1.x ではこのマクロは `JISHO_FILE` と定義していたが，辞書ファイルとの名前の整合性を取るためにこのようにした．

2. 実行ファイル dbskkd-cdb をコンパイルするため make する． この際 TinyCDB 中の cdb.h が必要となる． また TinyCDB 中の libcdb.a をリンクする．

3. make install で dbskkd-cdb を実行ディレクトリにコピーする．既定値では /usr/local/libexec へコピーする．

## インストールの際の注意

このプログラム専用の非特権ユーザを設定し，そのユーザの権限で起動するようにすること．

★ root 権限で起動してはならない． ★

設定例は examples.md に記している．

## 注意・限界・改良の余地

dbskkd-cdb は一旦起動されると終了するまではcdb辞書ファイルをオープンしたままになっている． cdb ではオープンされたファイルの中身は変わらないことを仮定しているため， dbskkd-cdb を起動した状態で cdb 辞書ファイルの内容を変更してはならない． cdb 辞書ファイルを作り直す場合は， dbskkd-cdb を起動していないことを確認する必要がある． 個人的には SERVER\_NOT\_FOUND の時の挙動は無駄だと思うが，慣習として対応している．プロトコルの詳細については skk-server-protocol.md に記している．

dbskkd-cdb では，接続してきたクライアントの利用履歴を取ることはない．

## 配布条件

The MIT License に準拠します．詳しい条件については ファイル LICENSE の内容を参照してください．

## バグレポート，意見交換など

本プログラムに関する意見交換は情報共有のためGitHub 上のレポジトリ <https://github.com/jj1bdx/dbskkd-cdb/> のissueとして報告していただくようお願いします．日本語あるいは英語でお願いします．

## 謝辞

cdb の開発者 Daniel J. Bernstein および TinyCDB の開発者 Michael Tokarev の両氏に最大限の感謝と賛辞を贈ります．

また，本プログラムの開発にあたって，以下の方々，ここに列挙していない contributor の皆様，ならびに SKK の利用を通じて改良点などをご教示くださった皆様に感謝致します．

(敬称略)

佐藤 雅彦，中島 幹夫，亀山 幸義，佐藤 正章，阪本 崇，鵜飼 文敏，前野 年紀，本田 康弘，中原 早生，菊地 高広，まつもとひでお，鯉江 英隆，川村 尚生，木原 英人，稲員 力士，松下 誠，山縣 敦，木下 達也，中居 良介，梅本 肇

## 修正箇所について

過去の修正履歴については CHANGES.md および Git のコミットログを参照してください。

[End of memorandum]
