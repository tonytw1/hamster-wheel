FROM faucet/python3

RUN pip install requests

COPY reports/report.py .

CMD ["report.py"]
ENTRYPOINT ["python3"]
