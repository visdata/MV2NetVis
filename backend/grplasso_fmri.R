library(grplasso)
library(jsonlite)
args = commandArgs(T)
fid = args[1]
z_thes = args[2]
db_thes = args[3]
sub1 = args[4]
sub2 = args[5]

dataset = read_json(paste('cache_fmri/feature_',fid,z_thes,db_thes,sub1,sub2,'.json',sep=""), simplifyVector = TRUE)
x = dataset
print(ncol(x))
print(nrow(x))
y = read_json(paste('cache_fmri/label_',fid,z_thes,db_thes,sub1,sub2,'.json',sep=""), simplifyVector = TRUE)
index = read_json(paste('cache_fmri/index_',fid,z_thes,db_thes,sub1,sub2,'.json',sep=""), simplifyVector = TRUE)
lambda <- lambdamax(x, y = y, index = index, penscale = sqrt, model = LogReg()) * 0.5^(0:5)
fit <- grplasso(x, y = y, index = index, lambda = lambda, model = LogReg(), penscale = sqrt, control = grpl.control(update.hess = "lambda", trace = 0))
write_json(coefficients(fit)[,6],paste('cache_fmri/lass_coef_',fid,z_thes,db_thes,sub1,sub2,'.json',sep=""))
write_json(lambda,paste('cache_fmri/lambda_',fid,z_thes,db_thes,sub1,sub2,'.json',sep=""))

