SELECT * FROM Products
WHERE Price NOT BETWEEN 10 AND 20;

SELECT CustomerID AS ID, CustomerName AS Customer
FROM Customers;

SELECT * FROM Customers
WHERE Country='Mexico'; 

--Select all:
SELECT * FROM Customers;

CREATE TABLE Persons (
    PersonID int,
    LastName varchar(255),
    FirstName varchar(255),
    Address varchar(255),
    City varchar(255)
);
