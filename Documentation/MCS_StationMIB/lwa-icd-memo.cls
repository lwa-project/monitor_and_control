% lwa-icd-memo.cls - Compact LaTeX document class for LWA memos and short ICDs
%
% Based on the article class for continuous-flow layout.  Shares the metadata
% commands and macro ecosystem with lwa-icd.cls so configuration files are
% interchangeable, but renders in a more compact style suitable for shorter
% documents.
%
% Usage:
%   \documentclass[11pt,letterpaper]{lwa-icd-memo}
%
%   \renewcommand{\icdAbbrev}{MCS}
%   \renewcommand{\docVersion}{1.1}
%   ...
%
%   \begin{document}
%   \makeICDtitlepage
%   \tableofcontents
%   \newpage
%   \section{Introduction}
%   ...
%   \end{document}

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{lwa-icd-memo}[2026/02/14 v1.0 LWA ICD Memo class (compact)]

% ============================================================================
% Base class
% ============================================================================

\LoadClass[11pt,letterpaper]{article}

% ============================================================================
% Required packages
% ============================================================================

% Page layout
\RequirePackage[margin=1in,headheight=14pt]{geometry}

% Headers and footers
\RequirePackage{fancyhdr}

% Tables
\RequirePackage{longtable}
\RequirePackage{booktabs}
\RequirePackage{tabularx}
\RequirePackage{array}

% Callout / annotation boxes
\RequirePackage[most]{tcolorbox}

% Hyperlinks and cross-references
\RequirePackage{hyperref}

% Code listings
\RequirePackage{listings}
\lstset{
    basicstyle=\ttfamily\small,
    breaklines=true,
    frame=single,
    columns=fullflexible
}

% Graphics and colors
\RequirePackage{graphicx}
\RequirePackage{xcolor}

% Math and units
\RequirePackage{amsmath}
\RequirePackage{siunitx}

% Formatting
\RequirePackage{parskip}
\RequirePackage{enumitem}
\RequirePackage{underscore}

% ============================================================================
% Document metadata
%
% Same commands as lwa-icd.cls so config/metadata.tex files are compatible.
% ============================================================================

\newcommand{\icdAbbrev}{}                  % e.g., MCS, SSMIF, OBS
\newcommand{\docVersion}{}                 % e.g., 1.1, 5
\newcommand{\docDate}{}                    % e.g., February 2026
\newcommand{\prevVersion}{}                % e.g., 1.0
\newcommand{\prevDate}{}                   % e.g., April 4, 2009
\newcommand{\docAuthors}{}                 % e.g., S.~Ellingson
\newcommand{\docNumber}{}                  % e.g., MCS_Common_ICD
\newcommand{\docTitle}{}                   % Full title with line breaks
\newcommand{\docSubtitle}{Version \docVersion}
\newcommand{\orgName}{Long Wavelength Array Project}
\newcommand{\orgAddress}{%
    University of New Mexico\\
    Department of Physics and Astronomy\\
    Albuquerque, NM 87131
}
\newcommand{\sysEngApproval}{System Engineering}
\newcommand{\sysArchApproval}{System Architecture}

% ============================================================================
% Page style
% ============================================================================

\pagestyle{fancy}
\fancyhf{}
\fancyhead[L]{\icdAbbrev{} ICD}
\fancyhead[R]{Version \docVersion}
\fancyfoot[C]{\thepage}
\renewcommand{\headrulewidth}{0.4pt}
\renewcommand{\footrulewidth}{0.4pt}

% ============================================================================
% Hyperref setup (deferred so metadata can be set first)
% ============================================================================

\AtBeginDocument{%
    \hypersetup{
        colorlinks=true,
        linkcolor=blue,
        filecolor=magenta,
        urlcolor=cyan,
        citecolor=blue,
    }%
}

% ============================================================================
% Callout box environments (same as lwa-icd.cls)
% ============================================================================

% General note box (gray)
\newtcolorbox{noteBox}[1][]{
    colback=gray!10,
    colframe=gray!70,
    fonttitle=\bfseries,
    title={Note},
    #1
}

% Site-specific configuration note (orange)
\newtcolorbox{siteNote}[1][]{
    colback=orange!10,
    colframe=orange!70,
    fonttitle=\bfseries,
    title={Site-Specific Configuration},
    #1
}

% Generic "New in Version X" box (green)
\newtcolorbox{newInVersion}[2][]{
    colback=green!10,
    colframe=green!70,
    fonttitle=\bfseries,
    title={New in Version #2},
    #1
}

% Generic callout box with custom title and color
\newtcolorbox{calloutBox}[3][]{
    colback=#3!10,
    colframe=#3!70,
    fonttitle=\bfseries,
    title={#2},
    #1
}

% Factory command to create project-specific callout box environments.
\newcommand{\newicdcallout}[3]{%
    \newtcolorbox{#1}[1][]{%
        colback=#3!10,%
        colframe=#3!70,%
        fonttitle=\bfseries,%
        title={#2},%
        ##1%
    }%
}

% ============================================================================
% Inline formatting commands (same as lwa-icd.cls)
% ============================================================================

\newcommand{\newInVer}[1]{%
    \textnormal{\textcolor{green!70!black}{\textbf{[New in Version #1]}}}%
}

\newcommand{\sitemargin}[1]{\marginpar{\small\textcolor{orange}{Site: #1}}}

% Command name formatting (e.g., \cmd{PNG})
\newcommand{\cmd}[1]{\texttt{#1}}

% MIB entry formatting (e.g., \mib{SUMMARY})
\newcommand{\mib}[1]{\texttt{#1}}

% Hex value formatting (e.g., \hex{0A} produces 0x0A)
\newcommand{\hex}[1]{\texttt{0x#1}}

% Register/address formatting
\newcommand{\reg}[1]{\texttt{#1}}

% ============================================================================
% Title page (compact version - no approval block)
% ============================================================================

\newcommand{\makeICDtitlepage}{%
    \begin{titlepage}
        \centering
        \vspace*{1cm}
        \begin{center}
            \includegraphics[width=0.3\textwidth]{figures/logo}
        \end{center}
        \vspace{1.5cm}
        {\LARGE\bfseries \docTitle \par}
        \vspace{0.5cm}
        {\Large \docSubtitle \par}
        \vspace{2cm}
        {\large \docAuthors \par}
        \vspace{1cm}
        {\large \docDate \par}
        \vfill
        {\normalsize \orgName \par}
    \end{titlepage}%
}

% ============================================================================
% Front matter helpers
% ============================================================================

% Change record header (unnumbered section, added to TOC)
\newcommand{\changerecordheader}{%
    \section*{Change Record}%
    \addcontentsline{toc}{section}{Change Record}%
}

% Standard TOC sequence (compact - no separate List of Tables page)
\newcommand{\makeICDtoc}{%
    \tableofcontents
    \newpage
}

\endinput
