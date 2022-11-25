x = [1,2,3,4,5,6]
y1 = [6.32,4.43,5.44,6.4,2.11,5.11]
y2 = [2,3,4,3,5,4]
y3 = [123,234,345,542,667,885]
y4 = [200,250,310,340,370,400]
tiledlayout(2,1)
nexttile
loglog(x,y1,'-','Linewidth',2)
hold on
loglog(x,y2,'-','Linewidth',2)
hold off
xlabel('Problem Size (m+n)')
ylabel('Memory (KB)')
legend('Basic Algorithm','Efficient Algorithm')

nexttile
loglog(x,y3,'-','Linewidth',2)
hold on
loglog(x,y4,'-','Linewidth',2)
hold off
xlabel('Problem Size (m+n)')
ylabel('Time (milliseconds)')
legend('Basic Algorithm','Efficient Algorithm')