//Bitki Bakım Sistemi

#include <LiquidCrystal.h>//16x2 LCD Ekran kütüphanemizi yazılımımıza dahil ediyoruz.
LiquidCrystal lcd(12,11,5,4,3,2);//Ekran pin bağlantılarını tanımlıyoruz.

#include "DHT.h"//DHT11 Sıcaklık ve Nem Sensörünün kütüphanesini yazılımımıza dahil ediyoruz.
#define DHTPIN 6//DHT11'in bağlı olduğu pine atıyoruz.
#define DHTTYPE DHT11//DHT Sensörünün modelini giriyoruz.(DHT11,DHT22 gibi)
DHT SvN(DHTPIN,DHTTYPE);//DHT11 Sıcaklık ve Nem Sensörüne isim veriyoruz.Ben kısaltma olsun diye "Sıcaklık ve Nem" kelimelerinin baş harfleri olan "SvN" ismini verdim.

int Isik=1;//Fotodirenç sensörü için "Isik" adlı bir değişken tanımladık.
const int in1=7;// Su sistemini motor sürücü ile kontrol edmemiz için motor sürücünün 1 ayağına 7. pinden,   
const int in2=9; // 1 diğer ayağına ise 9.pinden güç beslemesi yapıyoruz.
const int Alarm=8;//Devreye bir adet uyarı için "Aktif Zil" ekliyoruz, zile "Alarm" ismi veriyoruz ve zilin bağlı olduğu pine atıyoruz.
const int aydinlatma=1; //Lcd ekranın arka plan aydınlatmasını bağlı olduğu pine atıyoruz.

void setup()//Kuruluma başlıyoruz.
{
  SvN.begin();//DHT11'in kurulumunu yapıyoruz.
  pinMode(aydinlatma, OUTPUT);//Lcd ekranı güç kaynağına göre çıkış pini olarak ayarlıyoruz.
  pinMode(A1,INPUT);//Fotodirenç sensörümüzün bağlı olduğu A1 pinini güç kaynağına göre giriş pini olarak ayarlıyoruz.
  pinMode(in1,OUTPUT);// Motor sürücüye giden pinleri çıkış pini olarak ayarlıyoruz.
  pinMode(in2,OUTPUT); // Motor sürücüye giden pinleri çıkış pini olarak atıyoruz.
  pinMode(Alarm,OUTPUT);//Uyarı pinini güç kaynağına göre çıkış pini olarak ayarlıyoruz.
  lcd.begin(16,2);//16x2 LCD Ekran kullandığımızı sisteme bildiriyoruz.
}

void loop()//Sistemin döngüsünü oluşturuyoruz.
{
  digitalWrite(aydinlatma,HIGH);//Lcd ekranı yak.
  float N = SvN.readHumidity();//DHT11'den Nem verilerini okutuyoruz ve okuttuğumuz verileri yine aynı satırda oluşturduğumuz Nem'in kısaltması olarak baş harfi olan "N" değişkenine atıyoruz.
  float C = SvN.readTemperature();//DHT11'den Sıcaklık verilerini okutuyoruz ve okuttuğumuz verileri yine aynı satırda oluşturduğumuz ve Celsius'un baş harfi olan "C" değişkenine atıyoruz.
  float f = SvN.readTemperature(true);//DHT11'in okuduğu sıcaklık verilerini doğruluyoruz.
  Isik=analogRead(A1);//"Isik" adlı değişkeni A1 pinine yani "Fotodirenç Sensörü"nün bağlı olduğu pine atıyoruz.
  Isik=map(Isik,0,1023,0,100);//"0-1023" arasında okunan ışık verilerini "0-100" aralığına çeviriyoruz.
  delay(1);//Sistemin çok kısa süreli dinlenmesi için 1 miliSaniye bekliyoruz.
  int TNem;//Toprak Nem Sensörü için "TNem" adlı bir değişken oluşturuyoruz.
  TNem=analogRead(A0);//"TNem" adlı değişkeni A0 pinine yani "Toprak Nem Sensörü"nün bağlı olduğu pine atıyoruz. 
  lcd.clear();//16x2 LCD Ekranı temizliyoruz.
  lcd.setCursor(0,0);//Ekranın birinci satırına
  lcd.print("Hava: ");//Sıcaklık verilerini yazdırıyoruz.
  lcd.print(C);
  lcd.print(" C");//Celcius işaretine en çok benzeyen işareti yazdırıyoruz.
  lcd.setCursor(0,1);//Ekranın ikinci satırına
  lcd.print("Nem : %");//Nem verilerini yazdırıyoruz.
  lcd.print(N);
  delay(6000);//6sn. bekliyoruz.
  lcd.clear();//16x2 LCD Ekranı tekrar temizliyoruz.
  lcd.setCursor(0,0);//Ekranın birinci satırına
  if(Isik>25)//Eğer Isik verisi 25'ten büyükse:
  {
    lcd.print("Gunduz");//"Gündüz" yazdır.
  }
  else//aksi taktirde
  {
    lcd.print("Gece");//"Gece" yazdır.
  }
  delay(6000);//Yazıları rahat okuyabilmek için 6sn. bekliyoruz.
  lcd.clear();//16x2 LCD Ekranı tekrar temizliyoruz.
  lcd.setCursor(0,0);//Ekranın birinci satırına
  
  if(TNem<=400)//Eğer Nem 400'den küçük veya eşitse:
  {
    lcd.print("Kuru Toprak");//"Kuru Toprak" yazdır.
    lcd.setCursor(0,1);//Ekranın ikinci satırına
    lcd.print("Sulaniyor...");//"Sulanıyor..." yazdır.
    digitalWrite(in1,HIGH);// Potansiyel Fark oluşması için 1 ayağına Güç veriyoruz.
    digitalWrite(in2,LOW);// Aynı şekilde "Potansiyel Fark" oluşması için 1 ayağına güç vermiyoruz.
    delay(1500);//1,5sn boyunca sulama işlemi yap.
    digitalWrite(in1,LOW);// Güç vermediğimiz pine güç vererek potansiyel farkı 0 yapıyoruz ve bu da motoru durduracaktır. 
    lcd.clear();//Ekranı temizle.
    lcd.setCursor(0,0);//Ekranın birinci satırına
    lcd.print("Bekleniyor...");//"Bekleniyor..." yazdır.            
  }
  else if(TNem<=600 && TNem>400)//Eğer Nem 600'den küçük eşitse ve 400'den büyükse:
  {
    lcd.print("Nemli Toprak");//"Nemli Toprak" yazdır.
    lcd.setCursor(0,1);//Ekranın ikinci satırına
    lcd.print("Uygun Ortam");//"Uygun Ortam" yazdır.
  }
  else//Aksi taktirde
  {
    lcd.print("Su Baskini.");//"Su Baskını" yazdır.
    lcd.setCursor(0,1);//Ekranın ikinci satırına
    lcd.print("Tahliye Yapiniz");//"Tahliye Yapınız" yazdır.
    for(int i=0; i<5; i++)//5 kere alttaki ilk 6 satırı tekrar et.
    {
     digitalWrite(Alarm,HIGH);//Alarmı aç.
     delay(1000);//Bir saniye bekle.
     digitalWrite(Alarm,LOW);//Alarmı kapat.
     delay(1000);//Bir saniye bekle.
    }
  }
  lcd.clear(); //Ekranı Temizle.
  lcd.print("Uyku Moduna");//Ekranın 1. satırına Uyku Moduna yazısını yaz.
  lcd.setCursor(0,1);//Ekranın 2. satırına
  lcd.print("Geciliyor.");//Sonra Geciliyor yazısını yaz.
  delay(3000);//3sn boyunca ekranda yazdır.
  lcd.clear();//Ekranı Temizle
  delay(1500);//1.5sn bekle.
  lcd.print("Uyku Moduna");//Ekranın 1. satırına Uyku Moduna yazısını yaz.
  lcd.setCursor(0,1);//Ekranın 2. satırına
  lcd.print("Geciliyor.");//Sonra Geciliyor yazısını yaz.
  delay(3000);//3sn bekle.
  lcd.clear();//Ekranı Temizle.
  digitalWrite(aydinlatma, LOW);//Lcd Ekranın arka plan aydınlatmasını kapat.
  delay(21600000);//Sistemi 6saat dinlendiriyoruz. (180000=30dk 90000ms=15dk  2700000ms=45dk.)
}
