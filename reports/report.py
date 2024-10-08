import math
import os
import requests
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

api_base = "http://prometheus.default.svc.cluster.local:9090"
metric = "f267bffcaa98count"
wheel_diameter = 0.29
wheel_circumference = wheel_diameter * math.pi

message_from = os.environ.get('EMAIL_FROM')
message_to = os.environ.get('EMAIL_TO')
smtp_user = os.environ.get('SMTP_USER')
smtp_password = os.environ.get('SMTP_PASSWORD')
smtp_server = os.environ.get('SMTP_HOST')


def send_email(text):
    message = MIMEMultipart("alternative")
    message["Subject"] = "Hamster wheel activity"
    message["From"] = message_from
    message["To"] = message_to

    plain_part = MIMEText(text, "plain")
    message.attach(plain_part)

    smtp = smtplib.SMTP(smtp_server, 587)
    smtp.starttls()
    smtp.login(smtp_user, smtp_password)
    smtp.sendmail(message_from, message_to, message.as_string())


query = f"increase({metric}[24h])"
request_url = f"{api_base}/api/v1/query"
query_params = {'query': query}
r = requests.get(request_url, params=query_params)
if r.status_code == 200:
    increase = round(float(r.json()['data']['result'][0]['value'][1]))
    metres = round(increase * wheel_circumference)

    text = f"{increase} revolutions in the last 24 hours covering {metres} metres.\n"
    print(text)
    send_email(text)
    print("Done")
