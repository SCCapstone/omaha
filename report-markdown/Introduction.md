Since 2004, South Carolina has used Electronic Systems & Software's (ES&S) iVotronic direct-
recording electronic voting machine for its elections; however, recent investigations, such as the
*Evaluation and Validation of Election-Related Equipment, Standards and Testing* (EVEREST) Report
for then-Ohio Secretary of State Jennifer Brunner, have highlighted serious security flaws in both
the design of the voting machines&mdash;such as weak, easy-to-guess passwords and easily-
intercepted communications protocols&mdash; and in their implementation, which have cast doubts
upon the integrity of the voting process in South Carolina.

*Project Omaha* is a student-led project out of the College of Engineering and Computing at the
University of South Carolina, that was tasked with investigating
the hardware of the iVotronic and the related Personalized Electronic Ballot (PEB) cartridges in
order to determine possible attack vectors and, if possible, to try and exploit those vulnerabilities
to compromise the systems. This website serves as documentation of what we, the team members, have
accomplished over the course of the year; however, this was a purely black box operation: the team was
given the hardware without any accompanying software or other documentation outside of what could be
gathered from prior reports and papers.
 
Although we were originally assigned to take a hardware inventory of the iVotronic voting
terminal, to dump the iVotronic's ROM for further inspection, and to look for an exploit vector, early
setbacks forced us to switch over to the two PEBs. We believe that we have set up a strong foundation
in this website for anyone looking to pick up and continue this project. In addition to the ES&S hardware
provided, we acquired additional tools to facilitate our investigations, and we a large knowledge base
documenting almost all of the chips inside all of the available ES&S components. We have also uploaded
our methodology on getting ROM information out of the PEB itself, documenting what did and didn't work,
as well as suggestions for what to do next.

The *Project Omaha* team members are

* Dr. Duncan A. Buell, Professor, Dept. of Computer Science and Engineering, *Project Omaha* Supervisor

* Philip Conrad, Senior, Computer Science

* Zachary Hughes, Senior, Computer Science (left in December)

* Alexander Jordan, Senior, Computer Science

* Myndert Papenhuyzen, Senior, Applied Mathematics and Computer Science
	
### Table of Contents ###

* *[iVotronic Operating Environment](operating_env.html)* &ndash; An overview of the election day procedures
and how the iVotronic and PEB fit into the Election-Day pipeline.

* *[Disassembly Instructions](disassembly.html)* &ndash; Procedures for getting down to the hardware and 
finding your way around the circuit boards.

* *[Hacking the PEB](peb_hacking.html)* &ndash; A detailed look at what we did to the PEBs.

* *[Possible Attack Vectors](attack.html)* &ndash; Discussions on how the integrity of the iVotronic, PEB, and
elections can be compromised, compiled from our findings and chapter nine of the *EVEREST* report.

* *[For Future Reference](future_ref.html)* &ndash; Information for any future teams so that they can pick
up where we left off.

    * *[PEB Hardware Reference](peb.html)* &ndash; A visual breakdown of the PEB, with datasheet links for
    the major components.

    * *[iVotronic Hardware Reference](ivo_main.html)* &ndash; A visual breakdown of the iVotronic, with
    datasheet links for the major components. 