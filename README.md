# objectdetection_final

## Setup
#### C++'a Opencv'yi kurmak için şu kaynakları takip edebilirsiniz

https://www.youtube.com/watch?v=uCKX_FCg9Rk

https://medium.com/analytics-vidhya/how-to-install-opencv-for-visual-studio-code-using-ubuntu-os-9398b2f32d53

#### Yolo weights ve cfg dosyalalarını indirmek için
https://pjreddie.com/darknet/yolo/

[coco.names](https://github.com/pjreddie/darknet/blob/master/data/coco.names)

#### Qt kurulumu için

https://www.youtube.com/watch?v=2kSFwPXauMg&t=412s

Opencv versiyonu: 4.0
Qt versiyonu: 5.12.12


#### Çalıştırmak için

```
make server
./server
python client.py
```

## Output

![image](https://github.com/roboworks0/objectdetection_final/assets/37713845/25a687eb-d37a-4254-bd01-f02e3def412c)




## Yapılacaklar
- [ ] Nvidia bir bilgisayar CUDA kurulacak ve görüntü işlemenin performansı arttırılacak
- [ ] Opencv ile açılan bu frame QT ile yazdığımız masaüstü uygulamasına embed edilecek, [kod](https://github.com/yunus-temurlenk/Send-Opencv-Images-to-Qml-QPainter)

Yukarıdaki uygulamadan ekran görüntüsü
![image](https://github.com/roboworks0/objectdetection_final/assets/37713845/b4f51766-2c51-4f15-a409-132984c425a2)

Görüntü işlemeyi yukarıdaki uygulamaya bağlamak gerekiyor





