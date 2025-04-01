### PICO9918DVI ソフトウェア

PICO9918 のファームウェアの VGA 部分を、PicoDVI ライブラリに交換して、HDMIディスプレイへの出力に対応させました。  
元になっている PICO9918 のファームウェアは v0.4.3 です。  
PICO9918DVI のメインのコードは Arduino 言語で記述、Arduino IDE を使用してコンパイル(ビルド)を行います。  
(なぜ Arduino 言語で記述しているのか、それは**私があまりに知識経験不足すぎて Arduino IDE しか扱えない**からです。ただそれだけ)

### オリジナルの PICO9918 との相違点

・スキャンラインの表示が未実装です。  
・電源投入時のロゴのアニメーションがありません。  
・F18Aモードは未実装です。

### ソースコードのコンパイル（ビルド）方法
1. **Arduino IDE** を PC にインストール。
2. Arduino IDE のボードマネージャで **Raspberry Pi Pico/RP2040/RP2350** をインストールして、Arduino IDE で Raspberry Pi Pico 2 を扱えるようにする。   
3. Arduino IDE のライブラリマネージャーで **PicoDVI - Adafruit Fork** をインストール。<br/>
 **※** 但し **PicoDVI - Adafruit Fork** の **1.3.0** では、更新された内容が反映されていない可能性があります(多分)、なので、私は自分用にフォークして、自分なりに変更を適用したライブラリを使用しています。今のところは問題なく使えています。但し無保証です。<br/>
 [https://github.com/HisashiKato/PicoDVI](https://github.com/HisashiKato/PicoDVI)<br/>
  これを使う場合は、いったんライブラリマネージャーで **PicoDVI - Adafruit Fork** を削除、私のフォークのリポジトリの Code から **Download ZIP** を選んでダウンロードして、Arduino IDE の スケッチ > ライブラリをインクルード > .ZIP形式のライブラリをインストール でダウンロードしたZIPを選択。<br/>
4. ファームウェアのコードをダウンロード。<br/>
　**pico9918dvi_MSX** のリポジトリの Code から **Download ZIP** を選んでダウンロードして、任意のフォルダに展開。  
5. 試しにコンパイルしてみる。<br/>
　Arduino IDE の ファイル > 開く で展開したフォルダの Software/pico9918dvi_0_0_1/pico9918dvi_0_0_1.ino を選択。<br/>
　Arduino IDE の ツール で、<br/>
　　**ボード: "Raspberry Pi Pico 2"**<br/>
　　**CPU Architecture: "ARM"**<br/>
　　**Optimize: "Optimize Even More (-O3)"**<br/>
　　**USB Stack: "No USB"**<br/>
　と設定して、**検証**(VERIFY)、エラーが出なければOK  
6. Pico 2 に書き込む。書き込み(UPLOAD)  


以上？
