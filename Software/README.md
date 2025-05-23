### PICO9918DVI ソフトウェア

PICO9918 のファームウェアの VGA 部分を PicoDVI ライブラリに交換して HDMIディスプレイへの出力に対応させました。  
元になっている PICO9918 のファームウェアは v0.4.4 です。  
PICO9918DVI のメインのコードは Arduino 言語で記述。Arduino IDE を使用してコンパイル(ビルド)を行います。  
(なぜ Arduino 言語で記述しているのか、それは**私があまりに知識経験不足すぎて Arduino IDE しか扱えない**からです。ただそれだけ)

### オリジナルの PICO9918 との相違点

・スキャンラインの表示が未実装です。  
・電源投入時のロゴのアニメーションがありません。  
・F18Aモードは未対応です。

### ソースコードのコンパイル（ビルド）方法
1. **Arduino IDE** を PC にインストール。
2. Arduino IDE のボードマネージャに [ **Raspberry Pi Pico/RP2040/RP2350 ”Arduino Pico” (EarlePhilhower版)** ](https://github.com/earlephilhower/arduino-pico) を追加、インストール。インストール方法は[こちら](https://github.com/earlephilhower/arduino-pico#installation)
3. Arduino IDE でLチカ Blink.ino 等を Pico 2 ボードに書き込んで動作確認、Arduino IDE で Raspberry Pi Pico 2 を扱えるようにしておく。   
4. Arduino IDE のライブラリマネージャーで **PicoDVI - Adafruit Fork** をインストール。<br/>
 **※** 但し **PicoDVI - Adafruit Fork** の **1.3.0** では、更新された内容が反映されていない可能性があります(多分)、なので、私は自分用にフォークして、自分なりに変更を適用したライブラリを使用しています。今のところは問題なく使えています。(但し無保証)<br/>
 [https://github.com/HisashiKato/PicoDVI](https://github.com/HisashiKato/PicoDVI)<br/>
  これを使う場合は、いったんライブラリマネージャーで **PicoDVI - Adafruit Fork** を削除、私のフォークのリポジトリの Code から **Download ZIP** を選んでダウンロードして、Arduino IDE の スケッチ > ライブラリをインクルード > .ZIP形式のライブラリをインストール でダウンロードしたZIPを選択。<br/>
5. ファームウェアのコードをダウンロード。<br/>
　**pico9918dvi_MSX** のリポジトリの Code から **Download ZIP** を選んでダウンロードして、任意のフォルダに展開。  
6. 試しにコンパイルしてみる。<br/>
　Arduino IDE の ファイル > 開く で展開したフォルダの Software/pico9918dvi/pico9918dvi.ino を選択。<br/>
　Arduino IDE の ツール で、<br/>
　　**ボード: "Raspberry Pi Pico 2"**<br/>
　　**CPU Architecture: "ARM"**<br/>
　　**Optimize: "Optimize Even More (-O3)"**<br/>
　　**USB Stack: "No USB"**<br/>
　と設定して、**検証**(VERIFY)、エラーが出なければOK  
7. Pico 2 に書き込む。<br/>
　Raspberry Pi Pico 2 の BOOTSEL ボタンを押しながら、PC からの USBケーブルを Pico 2 に繋ぐ。<br/>
　Arduino IDE の ツール で **ボード: "UF2_board"** を選択して、**書き込み**(UPLOAD)<br/><br/>

　**※** コンパイル済みの [UF2](./UF2) を置いておきます。<br/><br/><br/>
　**※** 少し加筆して Ver.0.0.4 に上げました。(MAY 24 2025)<br/>
 　　（Ver.0.0.3 は欠番です）<br/><br/>

  
### 注意事項 ###
`xxx.pio` を書き換えたときは、**pioasm** を使用して手動で `xxx.pio` ファイルから `xxx.pio.h` ファイルを生成します。  
PicoDVI ライブラリの動作確認で、プログラム例(Examples) を使用する際は、以下の cfg の変更を忘れずに（毎回忘れるよく忘れる）<br/>
　例：  DVIGFX8 display(DVI_RES_320x240p60, true, xxxxxxxx_cfg);<br/>
ピン設定等で使われる xxxxxxxx_cfg は、  
x:\Users\xxxxxxx(UserName)\Documents\Arduino\libraries\PicoDVI_-_Adafruit_Fork\software\include\common_dvi_pin_configs.h  
に記述されています。  
あと、Pico と DVIコネクタとの結線の間違いも、よくやっちゃいます、注意。<br/><br/>

以上？
