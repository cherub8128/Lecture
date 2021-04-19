#selenium의 web driver 라이브러리를 불러온다.
from selenium import webdriver

#driver 변수에 웹드라이버를 불러오기(크롬)
driver = webdriver.Chrome()
#가져올 URL
URL='https://www.google.com'
#url= 해당 경로의 웹페이지를 web driver를 이용해 브라우저에 띄운다.
driver.get(url=URL)