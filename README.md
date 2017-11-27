# 8051-project

此repository主要是放上我的c code for 8051開發板(TE-8051A)  
並學習如何利用AT89S52晶片來控制PDIUSBD12晶片(USB晶片)  
<img src="https://raw.githubusercontent.com/XassassinXsaberX/8051-project/master/img/TE-8051A.jpg" height="500px" />  
(上圖是TE-8051A，可參考此[賣場連結](http://goods.ruten.com.tw/item/show?11090706500841))  
</br>
<img src="https://raw.githubusercontent.com/XassassinXsaberX/8051-project/master/img/PDIUSBD12.jpg" height="500px" />  
(上圖中的紅色開發板為USB開發板，可參考此[賣場連結](http://goods.ruten.com.tw/item/show?21405202337357))

## 參考書
"專題製作(單晶片8051/8951 C語言)"  
"圈圈教你玩USB(第二版)"  

## 參考接線方式
該紅色的USB開發板(採用PDIUSBD12晶片)與8051開發板TE-8051A(採用AT89S52 IC)  
的接線方式可參考如下  
D0～D7 <--------------> P2.0～P2.7  
 GND <--------------> GND  
 A0  <--------------> P3.5/T1  
 WR  <--------------> P3.6/WR  
 RD  <--------------> P3.7/RD  
 INT <--------------> P3.2/INT0  
