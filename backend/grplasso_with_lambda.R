library(grplasso)
library(jsonlite)
args = commandArgs(T)
fid = args[1]
z_thes = args[2]
db_thes = args[3]
sub1 = args[4]
sub2 = args[5]
lambda = args[6]
lambda = as.numeric(lambda)
lambda = c(lambda)
dataset = read_json(paste('cache/feature_',fid,z_thes,db_thes,sub1,sub2,'.json',sep=""), simplifyVector = TRUE)
x = dataset
print(ncol(x))
print(nrow(x))
print(lambda)
y = read_json(paste('cache/label_',fid,z_thes,db_thes,sub1,sub2,'.json',sep=""), simplifyVector = TRUE)
index = read_json(paste('cache/index_',fid,z_thes,db_thes,sub1,sub2,'.json',sep=""), simplifyVector = TRUE)

fit <- grplasso(x, y = y, index = index, lambda = lambda, model = LogReg(), penscale = sqrt, control = grpl.control(update.hess = "lambda", trace = 0))
write_json(coefficients(fit),paste('cache/lass_coef_',fid,z_thes,db_thes,sub1,sub2,'.json',sep=""))



