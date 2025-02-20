# CircularLogging

Diğer proje uygulamalarına eklenebilecek şekilde bir loglama sınıfı 
oluşturulacaktır. Bu loglama sınıfı ile günlük, saatlik, dakikalık ve saniyelik loglama 
yapılabilecek, her tarih için ayrı bir log dosyası oluşturulacaktır. Log dosyalarının 
isimlendirilmesi o tarihin UTC formatı olacaktır. 

## Config Dosyası İçeriği
Loglama ayarlarını, kod bir defa çalıştırıldıktan sonra oluşturulan config dosyası üzerinden değiştirebilirsiniz.
Şu anda sistemin bize verdiği ayarlar şunlar:
### Log_Path = "./log/'
Log dosyalarının nereye kaydedileceğini belirler
### Logging_Interval = "0T00-00-00"
Log dosyalarinin ne aralıkla oluşturulacağını belirler.
Formatı [Gün]T[Saat]-[Dakika]-[Saniye] olarak yazılır. 
Gün istenilen kadar sayıda basamak içerebilir fakar saat, dakika ve saniye her zaman 2 basamaklı olmalıdır
### Logging_Type = "log"
Log dosyalarının isminin sonuna nokta ile eklenecek dosya tipini belirler. Eğer "log" yazılmış ise dosya ismi ".log" ile biter
### Max_Log = 5
Belirtilen loglama yerinde en fazla olmasını istediğimiz dosya sayısını belirler. 
Loglanın dosya sayısı bu sayıyı aşarsa en eski dosyadan başlayarak bu sayıya ulaşanak kadar dosyaları siler.

## Basit Örnek
### Örnek Kod
    CircLog cl;

    cl.log("Code Started");
    
    if(funciton() == error){
        cl.log("Some Error on \'Function\'", ERROR);
    }
    
    if(is_contain(something, somewhere)){
        cl.log("somewhere not contain something", WARNING);
    }

    cl.log("End", "system");
### Örnek Çıktı ./log/2025-02-20T15-30-0.log
    [2025-02-20 15:30:5] [MESSAGE] Code Started
    [2025-02-20 15:30:5] [ERROR] Code Started
    [2025-02-20 15:30:5] [WARNING] Code Started
    [2025-02-20 15:30:5] [system] End


