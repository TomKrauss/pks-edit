
resource "aws_instance" "example" {
  	ami = "ami-a1b2c3d4"
	instance_type = "t2.micro"
  	network_interface {
		# Defines a network interface
	}
}

# default configuration
provider "google" {
	region = "us-central1"
}

# alternate configuration, whose alias is "europe"
provider "google" {
	alias  = "europe"
	region = "europe-west1"
}

resource "google_compute_instance" "example" {
	# This "provider" meta-argument selects the google provider
	# configuration whose alias is "europe", rather than the
	# default configuration.

	provider = google.europe
	for_each = tomap({
		a_group = "eastus"
		another_group = "westus2"
	})
	name = each.key
	location = each.value
}

data "aws_ami" "example" {
	id = var.aws_ami_id
	lifecycle {
		# The AMI ID must refer to an existing AMI that has the tag "nomad-server".
		postcondition {
			condition = self.tags["Component"] == "nomad-server"
			error_message = "tags[\"Component\"] must be \"nomad-server\"."
		}
	}
}
