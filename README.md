# PICO9918DVI for MSX

### 概要

これは、PICO9918 の VGA 部分を PicoDVI に交換して、HDMIディスプレイへの出力に対応させたものです。</br></br>

### PICO9918

PICO9918 は、Raspberry Pi Pico を使用して、昔のVDP TMS9918A を置き換えること (Drop-in replacement) を目的としたプロジェクトです。  
[Troy Schrapel](https://github.com/visrealm) 氏が開発しています。PICO9918 のプロジェクトのGitHubのページはこちら [pico9918](https://github.com/visrealm/pico9918)  
PICO9918 のTMS9918Aエミュレーションは、同じく Troy Schrapel 氏の [vrEmuts9918ライブラリ](https://github.com/visrealm/vrEmuTms9918) で処理されています。</br></br>
レトロゲーム機やレトロPCで使用されている VDP TMS9918A (TMS9918,TMS9928A,TMS9118 等) を外して PICO9918 に置き換えるだけで、VGA (アナログRGB 31KHz) の映像出力が可能になります。  
日本で TMS9918A シリーズを使っているレトロゲーム機やレトロPCは、セガ SG-1000,SC-3000, トミー ぴゅう太, ソード M5, 初代 MSX (MSX1) 等です。</br></br>
PICO9918 はオープンソースです。ファームウェアや基板のデータが公開されています。また Troy 氏が自ら製作した完成品のボードが海外のショップから購入出来ます(日本への発送が可能なショップです)。</br></br>

### PICO9918DVI for MSX

PICO9918 の VGA 部分を PicoDVI ライブラリに交換して、HDMIディスプレイへの出力に対応させてみました。  
Raspberry Pi Pico 2 を使用、レベルシフターを省略、直結します。</br></br>
**※** なぜ **for MSX** ?  
・今回 TMS9918A の MSX で使用するピンだけにしか対応してないので。  
・手元にある実機、MSX (カシオ MX-101) でしか動作実績が無いので。</br></br>

### 注意事項

私の環境では、一応それなりに動いています。ですが動作保証はしません。ユーザーサポートもしません。使用は自己責任で。何か問題があっても自分で何とかしてください。  
私は商用のプログラムのコード等は書いたことが無い素人で初心者なので、多分、色々と間違っていると思います。 ここのソースコード内の私が書き代えた部分をプロの人やC/C++の熟練者が見たら「なんでこんなコードの書き方をするんだよ！」と怒りさえ覚える人がいるかもしれません。 もし居ましたら、これをフォークして、添削や清書をして頂ければと思います。</br></br>

### ハードウェア

<img src="img/PICO9918DVIinMX101.jpg" width="600"></br></br>
**※** 私が唯一、所有している MSX カシオ(CASIO) MX-101 に組み込んでみました。</br></br>
<img src="img/PICO9918DVI_4.jpg" width="600"></br></br>
使用する部品は、基本、Raspberry Pi Pico 2 (その他、RP2350 搭載の Pico2 互換ボード) と、Pico DVI Sock (DVIコネクタ基板) だけです。  
詳しくは [PICO9918DVI ハードウェア](/Hardware/README.md) を読んでください。</br></br>

### ソフトウェア

オリジナルの PICO9918 のファームウェアの VGA 表示部分を外して、PicoDVIライブラリ (Raspberry Pi Pico シリーズ (RP2040,RP2350搭載ボード) で DVI (HDMI) 表示を可能にするライブラリ) を組み込みました。  
Arduino IDE を使用してコンパイル (ビルド) します。  
詳しくは [PICO9918DVI ソフトウェア](/Software/README.md) を読んでください。</br></br>

### 関連リンク

**PICO9918** [https://github.com/visrealm/pico9918](https://github.com/visrealm/pico9918)  
　素晴らしいプロダクトの開発、本当にありがとうございます。</br></br>
**PicoDVI - Adafruit Fork** の、さらにフォーク [https://github.com/HisashiKato/PicoDVI](https://github.com/HisashiKato/PicoDVI)  
　[PicoDVI - Adafruit Fork](https://github.com/adafruit/PicoDVI) の最新版 1.3.0 では更新されていない箇所があるので、それを適応するために自分用に作ったフォークです。これを使っています。</br></br>
**PICO9918 v0.4.2 for Pico2** [https://github.com/piigaa-densetu-two-dai/pico9918_for_pico2](https://github.com/piigaa-densetu-two-dai/pico9918_for_pico2)  
　[ピーガー伝説➋代](https://x.com/AOldfJaHcEmvAWC/)さんの[【壊れても知りません！】PICO9918 v0.4.1 for Pico2
物好きなDIY野郎専用](https://x.com/AOldfJaHcEmvAWC/status/1842423985910534281) を参考にさせていただきました。様々な拡張ボードを開発されている方で、毎回とても楽しく拝見させていただいています。

