import pandas as pd
import numpy as np
from sklearn.preprocessing import StandardScaler
from sklearn.neighbors import KNeighborsClassifier
from sklearn.metrics import accuracy_score
from sklearn.metrics import mean_absolute_error
from sklearn.metrics import mean_squared_error
import sklearn.model_selection as skl_ms
from math import sqrt


# Load training data
radioMap = pd.read_csv("RadioMapMatrixCombined.txt",sep=" ")

# Load test data
test = pd.read_csv("SaveMeasurement.txt", sep=" ")

# Separate features (RSSI values) from coordinates
X = radioMap.iloc[:, 1:-2]
y = radioMap.iloc[:, -2:]

# Separate features and coordinates for the test dataset
X_test = test.iloc[:, 1:-2]
y_test = test.iloc[:, -2:]

# Standardize features
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)
X_test_scaled = scaler.transform(X_test)  # Ensure the test set is scaled with the same scaler


n_fold = 10
cv = skl_ms.KFold(n_splits = n_fold, random_state =2, shuffle=True)
misclassification = 0

k = 5
training_mae = 0
validation_rmse = 0

for train_index, val_index in cv.split(X_scaled):
    X_train, X_val = X_scaled[train_index], X_scaled[val_index]
    y_train, y_val = y.iloc[train_index], y.iloc[val_index]
    
    model = KNeighborsClassifier(n_neighbors=k)
    model.fit(X_train,y_train)
    # Training predictions and MAE
    train_pred = model.predict(X_train)
    training_mae += mean_absolute_error(y_train, train_pred)
    
    # Validation predictions and RMSE
    val_pred = model.predict(X_val)
    validation_rmse += sqrt(mean_squared_error(y_val, val_pred))

# Average the errors across all folds
training_mae /= n_fold
validation_rmse /= n_fold

print(f"Training MAE for k={k}: {training_mae}")
print(f"Validation RMSE for k={k}: {validation_rmse}")

# Train the final model on the entire training dataset
final_model = KNeighborsClassifier(n_neighbors=k)
final_model.fit(X_scaled, y)

# Evaluate the model on the separate test dataset
y_test_pred = final_model.predict(X_test_scaled)

# Calculate test metrics
test_mae = mean_absolute_error(y_test, y_test_pred)
test_rmse = sqrt(mean_squared_error(y_test, y_test_pred))

# Calculate accuracy for each coordinate separately
accuracies = []
for i in range(y_test.shape[1]):
    accuracy = accuracy_score(y_test.iloc[:, i], y_test_pred[:, i])
    accuracies.append(accuracy)

# Calculate mean accuracy across all coordinates
mean_accuracy = np.mean(accuracies)

print(f"Test MAE: {test_mae}")
print(f"Test RMSE: {test_rmse}")
print(f"Test Accuracy: {mean_accuracy}")
