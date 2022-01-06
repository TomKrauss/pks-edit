from shutil import copyfile
copyfile("/root/a.txt", "/root/b.txt")

# Python program to print Even Numbers in given range

start = int(input("Enter the start of range: "))
end = int(input("Enter the end of range: "))

# iterating each number in list
for num in range(start, end + 1):
	
	# checking condition
	if num % 2 != 0:
		print(num, end = " ")

try:
    num = int(input())
    print(string+num)
except (TypeError, ValueError) as e:
    print(e)

with open("data_file.txt") as f:
    content_list = f.readlines()

# print the list
print(content_list)

# remove new line characters
content_list = [x.strip() for x in content_list]
print(content_list)
