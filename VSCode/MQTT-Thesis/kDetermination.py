import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

import sklearn.preprocessing as skl_pre
import sklearn.linear_model as skl_lm
import sklearn.discriminant_analysis as skl_da
import sklearn.neighbors as skl_nb
import sklearn.model_selection as skl_ms
from sklearn.preprocessing import StandardScaler

# Load training data
radioMap = pd.read_csv("RadioMapMatrixCombined.txt",sep=" ")

np.random.seed(1)
X = radioMap.iloc[:, 1:-2]
y = radioMap.iloc[:, -2:]
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)
print(X)

n_fold = 10
cv = skl_ms.KFold(n_splits = n_fold, random_state=2, shuffle = True)
K = np.arange(1,30)
misclassification = np.zeros(len(K))
for train_index, val_index in cv.split(X_scaled):
    X_train, X_val = X_scaled[train_index], X_scaled[val_index]
    y_train, y_val = y.iloc[train_index], y.iloc[val_index]
    
    for j, k in enumerate(K):
        model = skl_nb.KNeighborsClassifier(n_neighbors=k)
        model.fit(X_train,y_train)
        prediction = model.predict(X_val)
        misclassification[j] += np.mean(prediction != y_val)
        
misclassification /= n_fold
    
plt.plot(K, misclassification)
plt.title("cross validation error for kNN")
plt.xlabel("number of neighbors $k$")
plt.ylabel("Validation error")
plt.show()
