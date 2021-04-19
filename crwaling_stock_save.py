from selenium import webdriver
# 현재시간을 받아올 수 있는 라이브러리를 불러온다.
from datetime import datetime
# 시간 지연을 위한 라이브러리
import time

now = datetime.now()

URL='https://finance.naver.com/item/main.nhn?code=005930' #가져올 URL
# driver 변수에 크롬 web driver 불러오기
driver = webdriver.Chrome()
# 해당 URL로 접근
driver.get(URL)

# 무한 반복하기
while True:
    # 삼성 현재주가를 받아오기 위해 class 이름이 today인 값 찾아 저장하기
    today = driver.find_element_by_xpath("//div[@class='today']")
    # p 태그를 갖고 있는 모든 값을 찾아 저장하기
    p = today.find_element_by_tag_name("p")
    # p의 텍스트만 받아 줄바꿈을 없애고 출력하기
    samsung_stock = p.text.replace('\n','')
    # 현재시간을 저장하기
    now = datetime.now()
    text = f"{now.hour}:{now.minute}:{now.second},삼성전자,\"{samsung_stock}\"\n"
    # 파일을 추가 쓰기모드로 열어 text를 저장하고 닫는다.(없으면 만든다)
    with open(f"삼성주가 {now.year}년 {now.month}월 {now.day}일.txt",'a') as f:
        f.write(text)
    # n초 기다리기
    time.sleep(3)