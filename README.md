# 성경이를 위해 남겨놓는 글

![참고 링크](https://www.notion.so/Android-Face-Detection-using-OpenCV-ecf10ca7fba94023a7191949754355e1#80f4db74599b40cbbc97315fa9be53a3)

주의깊게 봐야할 코드는 아래 사진에 나와있는 MainActivity.java랑 native-lib.cpp입니다.

![Android program structure](https://github.com/Creative-Autonomous-Tractor/opencv-face-detection/blob/master/android_structure.png?raw=true)

native-lib.cpp가 OpenCV 관련된 처리를 다 해주는 부분이고, 그 내부에서도 detect() 함수 부분 (현재 65번째 줄부터 시작)만 건드리면 되는 부분입니다. 여기에 주석들을 몇 개 달아놨는데 참고하시면서 읽으시면 됩니다. 현재 특히 개선이 필요한 부분은 얼굴 인식의 정확도를 높이는 것입니다. detectMultiScale() 함수의 parameter 조정이 필요해보입니다.

MainActivity.java는 열화상 카메라랑 합쳐서 detect함수의 parameter로 열화상 카메라의 프레임이 대신 들어가도록 바꿔주면 됩니다.
