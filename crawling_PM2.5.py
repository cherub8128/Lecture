#selenim webdriver 라이브러리를 불러온다.
from selenium import webdriver

URL='https://www.accuweather.com/ko/kr/incheon/224032/air-quality-index/224032' #가져올 URL

#driver 변수에 크롬 web driver 불러오기
driver = webdriver.Chrome()
#최대 5초까지 로딩될 때까지 기다려주기 로딩이 되면 바로 넘어감
#driver.implicitly_wait(5)
"""
만일 usb 에러가 나면 다음과 같이 작성
options = webdriver.ChromeOptions()
options.add_experimental_option("excludeSwitches", ["enable-logging"])
driver = webdriver.Chrome(options=options)
"""
#해당 URL로 접근
driver.get(URL)

#div 태그로 시작하는 부분에서 data-qa가 airQualityPollutantPM2_5인 곳을 찾아 통째로 저장한다.
partOfPM2_5 = driver.find_element_by_xpath("//div[@data-qa='airQualityPollutantPM2_5']")
#다시 그 부분에서 class 이름이 pollutant-concentration라는 것을 찾아 저장한다.
pollutant_conc = partOfPM2_5.find_element_by_class_name("pollutant-concentration")
#그것의 텍스트를 출력한다.
print("현재시각 미세먼지 pm2.5의 농도: "+pollutant_conc.text)